/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Cpl/TShell/Dac/Cmd/Loop.h"


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;

///////////////////////////
Loop::Loop( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Command(commandList, "loop")
,m_state(eIDLE)
,m_level(0)
,m_breakLevel(0)
    {
    }

Loop::Loop( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command(commandList, "loop", ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
,m_state(eIDLE)
,m_level(0)
,m_breakLevel(0)
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T Loop::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars      = context.getVariables();
    Cpl::Text::String&  token     = context.getTokenBuffer();
    unsigned            numTokens = tokens.numParameters();
    // Error checking
    if ( numTokens < 2 )
        {
        m_state = eIDLE;
        return Command::eERROR_MISSING_ARGS;
        }

    // Process my state machine
    token = tokens.getParameter(1);
    switch( m_state )
        {
        case eIDLE:
            // Error checking
            if ( token != "WHILE" )
                {
                return Command::eERROR_INVALID_ARGS;
                }

            // Housekeeping    
            m_level      = 0;
            m_breakLevel = 0;
            m_state      = eCAPTURE_LOOP;
            context.endCommandCapture(); 
            context.endCommandReplay();

            // Start capture of the loop contents
            context.beginCommandCapture( m_level, rawInputString );
            context.enableFilter( *this );
            break;
            

        case eCAPTURE_LOOP:
            if ( token == "WHILE" )
                {
                m_level++;
                context.beginCommandCapture( m_level );
                context.enableFilter( *this );
                }
            else if ( token == "UNTIL" )
                {
                if ( m_level == 0 )
                    {
                    m_state = eLOOPING;
                    if ( context.endCommandCapture() )
                        {
                        if ( context.beginCommandReplay( m_level ) )
                            {
                            break; // If I get -->No errors where encounter during the capture of the loop contents.
                            }
                        }

                    // If I get here that is/was a problem with capture of the contents of the loop
                    m_state = eIDLE;
                    return Command::eERROR_FAILED;
                    }
                else
                    {
                    // FIXME: As of 8/21/2018
                    // FIXME: Believe a call to: context.enableFilter( *this ); is needed
                    // FIXME: Believe a call to: context.enableFilter( *this ); is needed
                    // FIXME: Believe a call to: context.enableFilter( *this ); is needed
                    m_level--;
                    }
                }
            else
                {
                m_state = eIDLE;
                return Command::eERROR_INVALID_ARGS;
                }
            break;
            

        case eLOOPING:
            if ( token == "WHILE" )
                {
                m_level++;

                // Process conditional clause when there is one
                if ( numTokens > 2 )
                    {
                    switch( conditional( context, tokens, 2, vars ) )
                        {
                        // Exit the loop if WHILE statement is false
                        case eFALSE:
                            m_state      = eBREAKING;
                            m_breakLevel = m_level;
							context.enableFilter(*this);
                            break;

                        case eERROR:
                            m_state = eIDLE;
                            return Command::eERROR_INVALID_ARGS;
                            break;

                        case eTRUE:
                            // Do nothing, i.e. continue looping
                            break;
                        }
                    }
                }
            else if ( token == "UNTIL" )
                {
                // Decrement my nest level now that I am bottom of the loop
                m_level--;
                
                // Process conditional clause when there is one
                if ( numTokens > 2 )
                    {
                    switch( conditional( context, tokens, 2, vars ) )
                        {
                        // Exit the current loop if UNTIL statement is true
                        case eTRUE:
                            if ( m_level == 0 )
                                {
                                context.endCommandReplay();
                                m_state = eIDLE;
                                }
                            break;

                        // Continue the loop
                        case eFALSE:
                            context.beginCommandReplay( m_level );
                            break;

                        case eERROR:
                            m_state = eIDLE;
                            return Command::eERROR_INVALID_ARGS;
                            break;
                        }
                    }

                else
                    {
                    // Continue the loop
                    context.beginCommandReplay( m_level );
                    }
                }
            else
                {
                // Reset the FSM on error
                m_state = eIDLE;
                return Command::eERROR_INVALID_ARGS;
                }
            break;


        case eBREAKING:
            if ( token == "WHILE" )
                {
                m_level++;
                context.enableFilter( *this );
                }
            else if ( token == "UNTIL" )
                {
                m_level--;

                // Exiting outer most loop
                if ( m_level == 0 )
                    {
                    context.endCommandReplay();
                    m_state = eIDLE;
                    }

                // Exiting an inner loop
                else if ( m_level < m_breakLevel )
                    {
                    m_state = eLOOPING;
                    }

                // Skipping over inner loop(s)
                else
                    {
                    context.enableFilter( *this );
                    }
                }
            else
                {
                // Reset the FSM on error
                m_state = eIDLE;
                return Command::eERROR_INVALID_ARGS;
                }
            break;
        }


    // If I get the command succeeded!
    return Command::eSUCCESS;
    }



