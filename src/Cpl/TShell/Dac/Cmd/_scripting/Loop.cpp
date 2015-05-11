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
                context.endCommandCapture();
                if ( m_level == 0 )
                    {
                    m_state = eLOOPING;
                    context.beginCommandReplay( m_level );
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
            if ( token == "BEGIN" )
                {
                m_level++;
                }
            else if ( token == "BREAK" )
                {
                m_state = eBREAKING;
                context.enableFilter( *this );
                }
            else if ( token == "UNTIL" )
                {
                context.beginCommandReplay(m_level);
                }
            else
                {
                m_state = eIDLE;
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
                m_state = eIDLE;
                return Command_::eERROR_INVALID_ARGS;
                }
            break;



        case eCOMPARE_ERROR:
            m_state = eIDLE;
            return Command_::eERROR_FAILED;
        }

    // If I get the command succeeded!
    return Command_::eSUCCESS;
    }



