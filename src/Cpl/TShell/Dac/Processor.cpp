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

static const char* resultToString_( Command::Result_T errcode );


///////////////////////////////////
Processor::Processor( Cpl::Container::Map<Command>&     commands,
                      ActiveVariablesApi&               variables,
                      Cpl::Text::Frame::StreamDecoder&  deframer,
                      Cpl::Text::Frame::StreamEncoder&  framer,
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
,m_outLock( outputLock )
,m_cmdBuffer( cmdBufferPtr )
,m_cmdBufSize( maxBufferCmds )
,m_comment( commentChar )
,m_esc( argEscape )
,m_del( argDelimiter )
,m_quote( argQuote )
,m_term( argTerminator )
,m_errorLevel( OPTION_CPL_TSHELL_DAC_PROCESSOR_NAME_ERRORLEVEL )
,m_lastCmdOutput( OPTION_CPL_TSHELL_DAC_PROCESSOR_NAME_LAST_OUTPUT, m_lastCmdOutputValue )
,m_last(0)
,m_running( false )
,m_filtering( false )
,m_filterMarker( 0 )
,m_replayCount( 0 )
,m_captureCount( 0 )
,m_currentIdx( 0 )
,m_replayIdx( 0 )
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
    m_outputBuffer.clear();
    m_framer.setOutput( outfd );

    // Add my system variables (but make sure they don't get add twice if the processor is 'restarted')
    if ( m_variables.find( m_errorLevel ) == 0 )
        {
        m_variables.addSystem( m_errorLevel );
        }
    if ( m_variables.find( m_lastCmdOutput ) == 0 )
        {
        m_variables.addSystem( m_lastCmdOutput );
        }

    // Output the greeting message
    greeting( outfd );
    m_deframer.setInput( infd );

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
            if ( !m_deframer.scan( OPTION_CPL_TSHELL_DAC_PROCESSOR_INPUT_SIZE, m_inputBuffer, frameSize ) )
                {
                // Error reading raw input -->exit the Command processor
                return false;
                }
            
            // Ensure the raw input buffer gets null terminated
            m_inputBuffer[frameSize] = '\0';

            // Cache the raw command input WHEN in capture mode
            if ( m_captureCount != 0 )
                {
                // Check for exceeding the command buffer                               
                if ( m_currentIdx >= m_cmdBufSize )
                    {
                    // This is bad (but not fatal, well maybe not fatal)
                    m_currentIdx   = 0;
                    m_captureCount = -1; // Set error-state to be returned when endCapture() is called
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
        Command::Result_T result = executeCommand( m_inputBuffer, outfd, m_captureCount );
        m_errorLevel.setValue ( (long) result );
        if ( result == Command::eERROR_IO )
            {
            // Output stream error -->exit Command Processlr
            return false;
            }

        // Capture last output
        m_lastCmdOutputValue = m_last;
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
Command::Result_T Processor::executeCommand( const char* deframedInput, Cpl::Io::Output& outfd, unsigned capturing ) throw()
    {
    // Make a copy of the input and then tokenize/parse the input
    strcpy( m_inputCopy, deframedInput );
    Cpl::Text::Tokenizer::TextBlock tokens( m_inputCopy, m_del, m_term, m_quote, m_esc ); 

    // Skip something that doesn't parse
    if ( tokens.isValidTokens() == false )
        {
        backoutCaptureLine( capturing );
        return outputCommandError( Command::eERROR_BAD_SYNTAX, deframedInput )? Command::eERROR_BAD_SYNTAX: Command::eERROR_IO;
        }

    // Skip blank and comment lines
    if ( tokens.numParameters() == 0 || *(tokens.getParameter(0)) == m_comment )
        {
        backoutCaptureLine( capturing );
        return Command::eSUCCESS;
        }

    // Lookup the command to be executed
    Cpl::Container::KeyLiteralString verb( tokens.getParameter(0) );
    Command*                         cmdPtr;
    if ( (cmdPtr=m_commands.find(verb)) == 0 )
        {
        return outputCommandError( Command::eERROR_INVALID_CMD, deframedInput )? Command::eERROR_INVALID_CMD: Command::eERROR_IO;
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
            return Command::eSUCCESS;
            }
        }

    // Execute the found command
    Command::Result_T result = cmdPtr->execute( *this, tokens, deframedInput, outfd );    
    if ( result != Command::eSUCCESS )
        {
        return outputCommandError( result, deframedInput )? result: Command::eERROR_IO;
        }

    return result;
    }

///////////////////////////////////
Cpl::System::Mutex& Processor::getOutputLock() throw()
    {
    return m_outLock;
    }

Cpl::Container::Map<Command>& Processor::getCommands() throw()
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

VariableApi& Processor::getLastOutput() throw()
    {                     
    return m_lastCmdOutput;
    }


///////////////////////////////////
bool Processor::writeFrame( const char* text  ) throw()
    {
    // Cache last output
    m_last = text;

    // Encode and output the text
    bool io = true;
    m_outLock.lock();
    io &= m_framer.startFrame();
    io &= m_framer.output( text );
    io &= m_framer.endFrame();
    m_outLock.unlock();

    return io;
    }

bool Processor::writeFrame( const char* text, size_t maxBytes  ) throw()
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
bool Processor::beginCommandReplay( unsigned level ) throw()
    {
    // Trap out-of-range level/index
    if ( level >= OPTION_CPL_TSHELL_PROCESSOR_MAX_NESTED_LOOPS )
        {
        return false;
        }

    m_replayCount = 1;
    m_replayIdx   = m_startIndexes[level];
    return true;
    }

void Processor::endCommandReplay(void) throw()
    {
    m_replayCount = 0;
    }

bool Processor::beginCommandCapture( unsigned level, const char* firstCmd ) throw()
    {
    // Trap out-of-range level/index
    if ( level >= OPTION_CPL_TSHELL_PROCESSOR_MAX_NESTED_LOOPS )
        {
        return false;
        }

    // Reset beginning index
    if ( m_captureCount <= 0 )
        {
        m_currentIdx          = 0;
        m_startIndexes[level] = 0;
        m_captureCount        = 1;

        // Capture the actual command that initiated the capturing (when selected, i.e. firstCmd NOT a null pointer)
        if ( firstCmd )
            {
            // Check for exceeding the command buffer, aka the processor is NOT configured to support loops                               
            if ( m_cmdBufSize == 0 )
                {
                return false;
                }

            strcpy( m_cmdBuffer[m_currentIdx++], firstCmd );
            }
        }
    else
        {
        // Enable Capture mode and store the capture index
        m_captureCount        = 1;
        m_startIndexes[level] = m_currentIdx - 1;
        }


    return true;
    }

bool Processor::endCommandCapture(void) throw()
    {
    bool result = true;
    if ( m_captureCount < 0 )
        {
        result = false;
        }

    m_captureCount = 0;
    return result;
    }

void Processor::backoutCaptureLine( unsigned capturing ) throw()
    {
    if ( capturing )
        {
        m_currentIdx--;
        }
    }

///////////////////////////////////
void Processor::enableFilter( Command& marker ) throw()
    {
    m_filtering    = true;
    m_filterMarker = marker.getVerb();
    }


///////////////////////////////////
Cpl::Text::String& Processor::getOutputBuffer() throw()
    {
    return m_outputBuffer;
    }

Cpl::Text::String& Processor::getTokenBuffer() throw()
    {
    m_tokenBuffer.clear();
    return m_tokenBuffer;
    }

Cpl::Text::String& Processor::getTokenBuffer2() throw()
    {
    m_tokenBuffer2.clear();
    return m_tokenBuffer2;
    }

Cpl::Text::String& Processor::getLastOutValue() throw()
    {
    return m_lastCmdOutputValue;
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
bool Processor::outputCommandError( Command::Result_T result, const char* deframedInput ) throw()
    {
    bool io = true;

    m_outputBuffer.format( "ERROR: [%s]", deframedInput );
    io &= writeFrame( m_outputBuffer ); 
    io &= writeFrame( resultToString_(result) );

    return io;
    }

const char* resultToString_( Command::Result_T errcode )
    {
    switch( errcode )
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

