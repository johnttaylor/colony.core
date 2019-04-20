/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Processor.h"
#include "Cpl/System/GlobalLock.h"

///
using namespace Cpl::TShell;

static const char* resultToString_( Command::Result_T errcode );


///////////////////////////////////
Processor::Processor( Cpl::Container::Map<Command>&     commands,
	Cpl::Text::Frame::StreamDecoder&  deframer,
	Cpl::Text::Frame::StreamEncoder&  framer,
	Cpl::System::Mutex&               outputLock,
	char                              commentChar,
	char                              argEscape,
	char                              argDelimiter,
	char                              argQuote,
	char                              argTerminator
)
	:m_commands( commands )
	, m_deframer( deframer )
	, m_framer( framer )
	, m_outLock( outputLock )
	, m_comment( commentChar )
	, m_esc( argEscape )
	, m_del( argDelimiter )
	, m_quote( argQuote )
	, m_term( argTerminator )
	, m_running( false )
{
}


///////////////////////////////////
bool Processor::start( Cpl::Io::Input& infd, Cpl::Io::Output& outfd ) noexcept
{
	// Set my state to: running
	Cpl::System::GlobalLock::begin();
	m_running = true;
	Cpl::System::GlobalLock::end();


	// Housekeeping
	m_outputBuffer.clear();
	m_framer.setOutput( outfd );

	// Output the greeting message
	outfd.write( OPTION_CPL_TSHELL_PROCESSOR_GREETING );
	m_deframer.setInput( infd );

	// Run until I am requested to stop
	for ( ;;)
	{
		// Check for stop request
		Cpl::System::GlobalLock::begin();
		bool run = m_running;
		Cpl::System::GlobalLock::end();

		if ( !run )
		{
			outfd.write( OPTION_CPL_TSHELL_PROCESSOR_FAREWELL );
			Cpl::System::Api::sleep( 250 ); // Allow time for the farewell message to be outputted
			return true;
		}


		// Output the prompt
		outfd.write( OPTION_CPL_TSHELL_PROCESSOR_PROMPT );

		// Get the next command string from my input stream
		size_t frameSize = 0;
		if ( !m_deframer.scan( OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE, m_inputBuffer, frameSize ) )
		{
			// Error reading raw input -->exit the Command processor
			return false;
		}

		// Ensure the raw input buffer gets null terminated
		m_inputBuffer[frameSize] = '\0';


		// Execute the command
		Command::Result_T result = executeCommand( m_inputBuffer, outfd );
		if ( result == Command::eERROR_IO )
		{
			// Output stream error -->exit Command Processor
			return false;
		}
	}


	// I should never get here!
	return false;
}


void Processor::requestStop() noexcept
{
	Cpl::System::GlobalLock::begin();
	m_running = false;
	Cpl::System::GlobalLock::end();
}


///////////////////////////////////
Command::Result_T Processor::executeCommand( char* deframedInput, Cpl::Io::Output& outfd ) noexcept
{
	// Tokenize/parse the input (Note: This is a destructive parse)
	Cpl::Text::Tokenizer::TextBlock tokens( deframedInput, m_del, m_term, m_quote, m_esc );

	// Skip something that doesn't parse
	if ( tokens.isValidTokens() == false )
	{
		return outputCommandError( Command::eERROR_BAD_SYNTAX, deframedInput ) ? Command::eERROR_BAD_SYNTAX : Command::eERROR_IO;
	}

	// Skip blank and comment lines
	if ( tokens.numParameters() == 0 || *( tokens.getParameter( 0 ) ) == m_comment )
	{
		return Command::eSUCCESS;
	}

	// Lookup the command to be executed
	Cpl::Container::KeyLiteralString verb( tokens.getParameter( 0 ) );
	Command*                         cmdPtr;
	if ( ( cmdPtr=m_commands.find( verb ) ) == 0 )
	{
		return outputCommandError( Command::eERROR_INVALID_CMD, deframedInput ) ? Command::eERROR_INVALID_CMD : Command::eERROR_IO;
	}

	// Execute the found command
	Command::Result_T result = cmdPtr->execute( *this, tokens, outfd );
	if ( result != Command::eSUCCESS )
	{
		return outputCommandError( result, deframedInput ) ? result : Command::eERROR_IO;
	}

	return result;
}

///////////////////////////////////
Cpl::Container::Map<Command>& Processor::getCommands() noexcept
{
	return m_commands;
}

bool Processor::writeFrame( const char* text ) noexcept
{
	// Encode and output the text
	bool io = true;
	m_outLock.lock();
	io &= m_framer.startFrame();
	io &= m_framer.output( text );
	io &= m_framer.endFrame();
	m_outLock.unlock();

	return io;
}

bool Processor::writeFrame( const char* text, size_t maxBytes ) noexcept
{
	// Encode and output the text
	bool io = true;
	m_outLock.lock();
	io &= m_framer.startFrame();
	io &= m_framer.output( text, maxBytes );
	io &= m_framer.endFrame();
	m_outLock.unlock();

	return io;
}


///////////////////////////////////
Cpl::Text::String& Processor::getOutputBuffer() noexcept
{
	return m_outputBuffer;
}

Cpl::Text::String& Processor::getTokenBuffer() noexcept
{
	m_tokenBuffer.clear();
	return m_tokenBuffer;
}

Cpl::Text::String& Processor::getTokenBuffer2() noexcept
{
	m_tokenBuffer2.clear();
	return m_tokenBuffer2;
}


const char* resultToString_( Command::Result_T errcode )
{
	switch ( errcode )
	{
	default:
		return "ERRNO ?: Command failed - unknown error code";

	case Command::eERROR_BAD_SYNTAX:
		return "ERRNO 1: Unable to parse command string";

	case Command::eERROR_INVALID_CMD:
		return "ERRNO 2: Command not supported";

	case Command::eERROR_IO:
		return "ERRNO 3: Input/Output stream IO encounter";

	case Command::eERROR_MISSING_ARGS:
		return "ERRNO 4: Command is missing argument(s)";

	case Command::eERROR_EXTRA_ARGS:
		return "ERRNO 5: Command encounter 'extra' argument(s)";

	case Command::eERROR_INVALID_ARGS:
		return "ERRNO 6: One or more Command arguments are incorrect/invalid";

	case Command::eERROR_FAILED:
		return "ERRNO 7: Command failed to complete one or more of its actions";
	}

	return "ERROR: I SHOULD NEVER GET HERE!";
}

bool Processor::outputCommandError( Command::Result_T result, const char* deframedInput ) noexcept
{
	bool io = true;

	m_outputBuffer.format( "ERROR: [%s]", deframedInput );
	io &= writeFrame( m_outputBuffer );
	io &= writeFrame( resultToString_( result ) );

	return io;
}