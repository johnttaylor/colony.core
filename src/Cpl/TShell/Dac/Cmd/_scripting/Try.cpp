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

#include "Cpl/TShell/Dac/Cmd/Try.h"
#include <string.h>

#include <stdio.h>


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;



///////////////////////////
Try::Try( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw()
:Command_(commandList, "try")
,m_state(eIDLE)
,m_level(0)
,m_stack(OPTION_CPL_TSHELL_DAC_CMD_TRY_IFELSE_NEST_LEVELS,m_memStack)
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command_::Result_T Try::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars  = context.getVariables();
    Cpl::Text::String&  token = context.getTokenBuffer();

    
    // Common Error checking
    if ( tokens.numParameters() == 1 )
        {
        m_state = eIDLE;
        return Command_::eERROR_MISSING_ARGS;
        }

    // Process my state machine
    token = tokens.getParameter(1);
printf( "CURRENT m_state=%d, m_level=%u cmd[%s]\n", m_state, m_level, rawInputString );

    switch( m_state )
        {
        case eIDLE:
            // Error checking
            if ( token != "IF" )
                {
                return Command_::eERROR_INVALID_ARGS;
                }

            m_stack.clearTheStack();
            m_level = 0;
            m_state = convert2State( conditional( context, tokens, 2, vars ) );
            if ( m_state == eCLAUSE_FALSE )
                {
                context.enableFilter( *this );
                }
            break;
            

        case eCLAUSE_FALSE:
            if ( token == "ENDIF" )
                {
                if ( m_level > m_stack.getNumItems() )
                    {
                    m_level--;
                    context.enableFilter( *this );
                    }
                else
                    {
                    m_state = eIDLE;
                    }
                }
            else if ( token == "ELSE" )
                {
                if ( m_stack.getNumItems() == m_level )
                     {
                     m_state = eCLAUSE_TRUE;
                     }
                 else
                     {
                     context.enableFilter( *this );
                     }
                }
            else if ( token == "ELIF" )
                {
                if ( m_stack.getNumItems() == m_level )
                    {
                    if ( (m_state = convert2State(conditional(context,tokens,2,vars))) == eCLAUSE_FALSE ) 
                        {
                        context.enableFilter( *this );
                        }
                    }
                else
                    {
                    context.enableFilter( *this );
                    }
                }
            else if ( token == "IF" )
                {
                m_level++;
                context.enableFilter( *this );
                }
            else
                {
                m_state = eIDLE;
                return Command_::eERROR_INVALID_ARGS;
                }
            break;
            

        case eCLAUSE_TRUE:
            if ( token == "ENDIF" )
                {
                if ( (m_state=popState()) != eIDLE )
                    {
                    m_level--;
                    context.enableFilter( *this );
                    }
                }
            else if ( token == "ELSE" )
                {
                context.enableFilter( *this );
                }
            else if ( token == "ELIF" )
                {
                context.enableFilter( *this );
                }
            else if ( token == "IF" )
                {
                if ( !m_stack.push( m_state ) )
                    {
                    m_state = eIDLE;
                    return Command_::eERROR_INVALID_ARGS;
                    }
                m_level++;
                if ( (m_state=convert2State(conditional(context,tokens,2,vars))) == eCLAUSE_FALSE )
                    {
                    context.enableFilter( *this );
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

printf( "NEW m_state=%d, m_level=%u\n", m_state, m_level );

    // If I get the command succeeded!
    return Command_::eSUCCESS;
    }




///////////////////////////
Try::State_T Try::popState() throw()
    {
    bool    popResult = true;
    State_T newState  = m_stack.pop( &popResult );
    if ( popResult == false )
        {
        newState = eIDLE;
        }

    return newState;
    }


Try::State_T Try::convert2State( Cpl::TShell::Dac::Cmd::Command_::CondResult_T result ) const throw()
    {
    switch( result )
        {
        case eTRUE:  return eCLAUSE_TRUE;
        case eFALSE: return eCLAUSE_FALSE;
        }
    
    return eCOMPARE_ERROR;
    }
