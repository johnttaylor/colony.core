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
Loop::Loop( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList, Try& tryCmd ) throw()
:Command_(commandList, "loop")
,m_state(eIDLE)
,m_level(0)
,m_tryCmd(tryCmd)
    {
    }

///////////////////////////
Cpl::TShell::Dac::Command_::Result_T Loop::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars  = context.getVariables();
    Cpl::Text::String&  token = context.getTokenBuffer();

    // Error checking
    if ( tokens.numParameters() < 2 )
        {
        return Command_::eERROR_MISSING_ARGS;
        }

    // Process my state machine
    token = tokens.getParameter(1);
    switch( m_state )
        {
        case eIDLE:
            // Error checking
            if ( token != "BEGIN" )
                {
                return Command_::eERROR_INVALID_ARGS;
                }

            m_level = 0;
            m_state = eCAPTURE_LOOP;
            context.beginCommandCapture( m_level );
            context.enableFilter( *this );
            break;
            

        case eCAPTURE_LOOP:
            if ( token == "BEGIN" )
                {
                m_level++;
                context.beginCommandCapture( m_level );
                context.enableFilter( *this );
                }
            else if ( token == "BREAK" )
                {
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
                context.endCommandCapture(); // Reset/Cancel capture when an error is encountered
                return Command_::eERROR_INVALID_ARGS;
                }
            break;
            


        case eLOOPING:
            if ( token == "BEGIN" )
                {
                m_level++;
                }
            else if ( token == "BREAK" )
                {
                m_state = eBREAKING;
                context.enableFilter( *this );

                // Handle the case of breaking out of IF/ELSE construct
                if ( m_tryCmd.breaking( this ) )
                    {
                    context.enableFilter( m_tryCmd );
                    }
                }
            else if ( token == "UNTIL" )
                {
                if ( !context.beginCommandReplay(m_level) )
                    {
                    // Reset the FSM on error
                    m_state = eIDLE;
                    context.endCommandReplay();
                    return Command_::eERROR_INVALID_ARGS;
                    }
                }
            else
                {
                // Reset the FSM on error
                m_state = eIDLE;
                context.endCommandReplay();
                return Command_::eERROR_INVALID_ARGS;
                }
            break;


        case eBREAKING:
            if ( token == "BEGIN" )
                {
                m_level++;
                context.enableFilter( *this );
                }
            else if ( token == "BREAK" )
                {
                // Ignore
                context.enableFilter( *this );
                }
            else if ( token == "UNTIL" )
                {
                if ( m_level == 0 )
                    {
                    context.endCommandReplay();
                    m_state = eIDLE;
                    }
                else
                    {
                    m_level--;
                    m_state = eLOOPING;
                    }
                }
            else
                {
                // Reset the FSM on error
                m_state = eIDLE;
                context.endCommandReplay();
                return Command_::eERROR_INVALID_ARGS;
                }
            break;



        case eCOMPARE_ERROR:
            m_state = eIDLE;
            context.endCommandCapture();
            context.endCommandReplay();
            return Command_::eERROR_FAILED;
        }

    // If I get the command succeeded!
    return Command_::eSUCCESS;
    }



