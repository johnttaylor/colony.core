/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Processor.h"


///
using namespace Cpl::TShell::Dac;

static const char* resultToString_( Command_::Result_T errcode );


///////////////////////////////////
Processor::Processor( Cpl::Container::Map<Command_>&    commands,
                      ActiveVariablesApi&               variables,
                      Cpl::Text::Frame::Decoder&        deframer,
                      Cpl::Text::Frame::Encoder&        framer,
                      Cpl::Text::String&                outputFrameBuffer,
                      Cpl::System::Mutex&               outputLock,
                      CommandBuffer_T*                  cmdBufferPtr,
                      unsigned                          maxBufferCmds,
                      char                              commentChar,
                      char                              argEscape,
                      char                              argDelimiter,
                      char                              argQuote,
                      char                              argTerminator,
                      Cpl::Log::Api&                    eventLogger
                    )
:m_commands( commands )
,m_variables( variables )
,m_deframer( deframer )
,m_framer( framer )
,m_outputFrameBuffer( outputFrameBuffer )
,m_outLock( outputLock )
,m_cmdBuffer( cmdBufferPtr )
,m_cmdBufSize( maxBufferCmds )
,m_comment( commentChar )
,m_esc( argEscape )
,m_del( argDelimiter )
,m_quote( argQuote )
,m_term( argTerminator )
,m_errorLevel( OPTION_CPL_TSHELL_DAC_PROCESSOR_NAME_ERRORLEVEL )
,m_running( false )
,m_filtering( false )
,m_filterMarker( 0 )
,m_replayCount( 0 )
,m_captureCount( 0 )
,m_currentIdx( 0 )
,m_replayIdx( 0 )
,m_startIndexes( sizeof(m_memStack)/sizeof(m_memStack[0]), m_memStack )
,m_logger( eventLogger )
    {
    }


///////////////////////////////////
bool Processor::start( Cpl::Io::Input& infd, Cpl::Io::Output& outfd ) throw()
    {
    // Set my state to: running
    m_fast.lock();
    m_running = true;
    m_fast.unlock();

    // Housekeeping
    m_filtering    = false;
    m_replayCount  = 0;
    m_captureCount = 0;
    m_currentIdx   = 0;
    m_replayIdx    = 0;
    m_startIndexes.clearTheStack();

    // Output the greeting message
    greeting( outfd );


    // Run until I am requested to stop
    for(;;)
        {
        // Check for stop request
        m_fast.lock();
        bool run = m_running;
        m_fast.unlock();
        if ( !run )
            {
            farewell( outfd );
            Cpl::System::Api::sleep(250); // Allow time for the farewell message to be outputted
            return true;
            }

        // Replay mode
        if ( m_replayCount )
            {
            // Get command to replay
            strcpy( m_inputBuffer, m_cmdBuffer[m_replayIdx] );

            // Check for end of command buffer    
            if ( ++m_replayIdx > m_currentIdx )
                {
                // I am 'out' of commands to replay -->so turn off replay mode
                endCommandReplay();
                }
            }

        // Live mode
        else
            {
            size_t frameSize = 0;

            // Output the prompt
            prompt( outfd );

            // Get the next command string from my input stream
            if ( !m_deframer.scan( infd, OPTION_CPL_TSHELL_DAC_PROCESSOR_INPUT_SIZE, m_inputBuffer, frameSize ) )
                {
                // Error reading raw input -->exit the Command processor
                return false;
                }
            
            // Ensure the raw input buffer gets null terminated
            m_inputBuffer[frameSize] = '\0';

            // Cache the raw command input WHEN in capture mode
            if ( m_captureCount )
                {
                // Check for exceeding the command buffer                               
                if ( m_currentIdx >= m_cmdBufSize )
                    {
                    // This is bad (but not fatal, well maybe not fatal)
                    m_currentIdx = 0;
                    m_logger.warning( "Cpl::TShell:DAC::Processor. Exceeded the command script buffer (idx=%ul, size=%ul).  Your DAC shell script commands are broken!", m_currentIdx, m_cmdBufSize );
                    }

                // Store the command
                else
                    {
                    strcpy( m_cmdBuffer[m_currentIdx], m_inputBuffer );
                    m_currentIdx++;
                    }
                }
            }


        // Execute the command
        Command_::Result_T result = executeCommand( m_inputBuffer, outfd );
        m_errorLevel.setValue ( (long) result );
        if ( result == Command_::eERROR_IO )
            {
            // Output stream error -->exit Command Processlr
            return false;
            }
        }
   

    // I should never get here!
    return false;
    }

    

void Processor::requestStop() throw()
    {
    m_fast.lock();
    m_running = false;
    m_fast.unlock();
    }


///////////////////////////////////
Command_::Result_T Processor::executeCommand( const char* deframedInput, Cpl::Io::Output& outfd ) throw()
    {
    // Make a copy of the input and then tokenize/parse the input
    strcpy( m_inputCopy, deframedInput );
    Cpl::Text::Tokenizer::TextBlock tokens( m_inputCopy, m_del, m_term, m_quote, m_esc ); 

    // Skip something that doesn't parse
    if ( tokens.isValidTokens() == false )
        {
        return outputCommandError( outfd, Command_::eERROR_BAD_SYNTAX, deframedInput )? Command_::eERROR_BAD_SYNTAX: Command_::eERROR_IO;
        }

    // Skip blank and comment lines
    if ( tokens.numParameters() == 0 || *(tokens.getParameter(0)) == m_comment )
        {
        return Command_::eSUCCESS;
        }

    // Lookup the command to be executed
    Cpl::Container::KeyLiteralString verb( tokens.getParameter(0) );
    Command_*                        cmdPtr;
    if ( (cmdPtr=m_commands.find(verb)) == 0 )
        {
        return outputCommandError( outfd, Command_::eERROR_INVALID_CMD, deframedInput )? Command_::eERROR_INVALID_CMD: Command_::eERROR_IO;
        }
    
    // Apply filtering (when enabled)
    if ( m_filtering )
        {
        // Turn off filtering when the filter marker is encounter.  Note: The filter marker command WILL BE executed
        if ( strcmp(tokens.getParameter(0), m_filterMarker) == 0 )
            {
            m_filtering = false;
            }
             
        // Skip the command
        else
            {
            return Command_::eSUCCESS;
            }
        }

    // Execute the found command
    Command_::Result_T result = cmdPtr->execute( *this, tokens, deframedInput, outfd );    
    if ( result != Command_::eSUCCESS )
        {
        return outputCommandError( outfd, result, deframedInput )? result: Command_::eERROR_IO;
        }

    return result;
    }

///////////////////////////////////
Cpl::System::Mutex& Processor::getOutputLock() throw()
    {
    return m_outLock;
    }

Cpl::Container::Map<Command_>& Processor::getCommands() throw()
    {
    return m_commands;
    }

ActiveVariablesApi& Processor::getVariables() throw()
    {
    return m_variables;
    }

VariableApi& Processor::getErrorLevel() throw()
    {
    return m_errorLevel;
    }

Cpl::Text::String& Processor::getOutputFrameBuffer() throw()
    {                     
    return m_outputFrameBuffer;
    }


///////////////////////////////////
void Processor::startOutput() throw()
    {
    m_framer.startFrame();
    }

void Processor::appendOutput( const char* text ) throw()
    {
    m_framer.output( text );
    }

void Processor::appendOutput( const char* text, size_t numBytes ) throw()
    {
    m_framer.output( text, numBytes );
    }

bool Processor::commitOutput( Cpl::Io::Output& outfd ) throw()
    {
    m_framer.endFrame();
    return outfd.write( m_outputFrameBuffer );
    }


///////////////////////////////////
void Processor::beginCommandReplay(void) throw()
    {
    bool status;

    m_replayCount++;
    m_startIndexes.peekTop( &status );
    if ( !status )
        {
        m_logger.warning( "Cpl::TShell:DAC::Processor. Start-of-Loop request invalid.  Your DAC shell script commands are broken!" );
        }
    }

void Processor::endCommandReplay(void) throw()
    {
    bool status;

    m_replayCount--;
    m_startIndexes.pop( &status );
    if ( !status )
        {
        m_logger.warning( "Cpl::TShell:DAC::Processor. End-of-Loop request invalid.  Your DAC shell script commands are broken!" );
        }
    }

void Processor::beginCommandCapture(void) throw()
    {
    if ( m_captureCount == 0 )
        {
        m_currentIdx = 0;
        }

    m_captureCount++;
    m_startIndexes.push( m_currentIdx );
    }

void Processor::endCommandCapture(void) throw()
    {
    m_captureCount--;
    }


///////////////////////////////////
void Processor::enableFilter( Command_& marker ) throw()
    {
    m_filtering    = true;
    m_filterMarker = marker.getVerb();
    }


///////////////////////////////////
Cpl::Text::String& Processor::getTokenBuffer() throw()
    {
    return m_tokenBuffer;
    }

Cpl::Text::String& Processor::getNumericBuffer() throw()
    {
    return m_numericWorkBuffer;
    }


///////////////////////////////////
bool Processor::greeting( Cpl::Io::Output& outfd ) throw()
	{
	return outfd.write( OPTION_CPL_TSHELL_DAC_PROCESSOR_GREETING );
	}
	
bool Processor::farewell( Cpl::Io::Output& outfd ) throw()
	{
	return outfd.write( OPTION_CPL_TSHELL_DAC_PROCESSOR_FAREWELL );
	}
	
bool Processor::prompt( Cpl::Io::Output& outfd ) throw()
	{
	return outfd.write( OPTION_CPL_TSHELL_DAC_PROCESSOR_PROMPT );
	}


///////////////////////////////////
bool Processor::outputCommandError( Cpl::Io::Output& outfd, Command_::Result_T result, const char* deframedInput ) throw()
    {
    bool io;

    io  = outputMessage( outfd, resultToString_(result) );
          startOutput();
          appendOutput( "[") ;
          appendOutput( deframedInput );
          appendOutput( "]" );
    io &= commitOutput( outfd );
    io &= outputMessage( outfd, " " );

    return io;
    }


const char* resultToString_( Command_::Result_T errcode )
    {
    switch( errcode )
        {
        default: 
            return "*** ERROR: Command failed - unknown error code";

        case Command_::eERROR_BAD_SYNTAX:
            return "*** ERROR: Unable to parse command string";

        case Command_::eERROR_INVALID_CMD:
            return "*** ERROR: Command not supported";

        case Command_::eERROR_IO:
            return "*** ERROR: Input/Output stream IO encounter";

        case Command_::eERROR_EXTRA_ARGS:
            return "*** ERROR: Command encounter 'extra' argument(s)";

        case Command_::eERROR_MISSING_ARGS:
            return "*** ERROR: Command is missing argument(s)";

        case Command_::eERROR_INVALID_ARGS:
            return "*** ERROR: One or more Command arguments are incorrect/invalid";

        case Command_::eERROR_FAILED:
            return "*** ERROR: Command failed to complete one or more of its actions";
        }

    return "**** I SHOULD NEVER GET HERE! ****";
    }

