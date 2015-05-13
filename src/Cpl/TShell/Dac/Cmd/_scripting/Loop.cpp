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

#include "Cpl/TShell/Dac/Cmd/Loop.h"



///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;

///////////////////////////
Loop::Loop( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw()
:Command_(commandList, "loop")
,m_state(eIDLE)
,m_level(0)
    {
    }

///////////////////////////
Cpl::TShell::Dac::Command_::Result_T Loop::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars      = context.getVariables();
    Cpl::Text::String&  token     = context.getTokenBuffer();
    unsigned            numTokens = tokens.numParameters();
    // Error checking
    if ( numTokens < 2 )
        {
        m_state = eIDLE;
        return Command_::eERROR_MISSING_ARGS;
        }

    // Process my state machine
    token = tokens.getParameter(1);
    switch( m_state )
        {
        case eIDLE:
            // Error checking
            if ( token != "WHILE" )
                {
                return Command_::eERROR_INVALID_ARGS;
                }

            // Housekeeping    
            m_level = 0;
            m_state = eCAPTURE_LOOP;
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
                    return Command_::eERROR_FAILED;
                    }
                else
                    {
                    m_level--;
                    }
                }
            else
                {
                m_state = eIDLE;
                return Command_::eERROR_INVALID_ARGS;
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
                            m_state      = eBREAKING_WHILE;
                            m_breakLevel = m_level;
							context.enableFilter(*this);
                            break;

                        case eERROR:
                            m_state = eIDLE;
                            return Command_::eERROR_INVALID_ARGS;
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
                        // Exit the loop if UNTIL statement is true
                        case eTRUE:
                            if ( m_level == 0 )
                                {
                                context.endCommandReplay();
                                m_state = eIDLE;
                                }
                            else
                                {
                                m_state = eBREAKING;
							    context.enableFilter(*this);
                                }
                            break;

                        // Continue the loop
                        case eFALSE:
                            context.beginCommandReplay( m_level );
                            break;

                        case eERROR:
                            m_state = eIDLE;
                            return Command_::eERROR_INVALID_ARGS;
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
                return Command_::eERROR_INVALID_ARGS;
                }
            break;


        case eBREAKING_WHILE:
            if ( token == "WHILE" )
                {
                m_level++;
                context.enableFilter( *this );
                }
            else if ( token == "UNTIL" )
                {
                m_level--;
                if ( m_level == 0 )
                    {
                    context.endCommandReplay();
                    m_state = eIDLE;
                    }
                if ( m_breakLevel < m_level )
                    {
                    context.enableFilter( *this );
                    }
                }
            else
                {
                // Reset the FSM on error
                m_state = eIDLE;
                return Command_::eERROR_INVALID_ARGS;
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
                if ( m_level == 0 )
                    {
                    context.endCommandReplay();
                    m_state = eIDLE;
                    }
                else
                    {
                    m_state = eLOOPING;
                    }
                }
            else
                {
                // Reset the FSM on error
                m_state = eIDLE;
                return Command_::eERROR_INVALID_ARGS;
                }
            break;
        }


    // If I get the command succeeded!
    return Command_::eSUCCESS;
    }



