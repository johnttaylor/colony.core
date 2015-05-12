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


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;



///////////////////////////
Try::Try( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw()
:Command_(commandList, "try")
,m_state(eIDLE)
,m_level(0)
,m_loopCmdPtr(0)
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
            m_state = compare( context, tokens, vars );
            if ( m_state == eCLAUSE_FALSE )
                {
                context.enableFilter( *this );
                }
            break;
            

        case eCLAUSE_FALSE:
            if ( token == "ENDIF" )
                {
                endLevelFalse( context );

                // Handle breaking out of loop
                checkForBreakout( context );
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

                // Handle breaking out of loop
                if ( checkForBreakout( context ) )
                    {
                    endLevelFalse( context, false );
                    }
                }
            else if ( token == "ELIF" )
                {
                if ( m_stack.getNumItems() == m_level )
                    {
                    if ( (m_state = compare(context,tokens,vars)) == eCLAUSE_FALSE ) 
                        {
                        context.enableFilter( *this );
                        }
                    }
                else
                    {
                    context.enableFilter( *this );
                    }
                
                // Handle breaking out of loop
                if ( checkForBreakout( context ) )
                    {
                    endLevelFalse( context, false );
                    }
                }
            else if ( token == "IF" )
                {
                if ( !checkForBreakout( context ) )
                    {
                    m_level++;
                    context.enableFilter( *this );
                    }
                else
                    {
                    // If I get here there is something is messed up, i.e. shouldn't/can't happen
                    m_state = eIDLE;
                    return Command_::eERROR_INVALID_ARGS;
                    }
                }
            else
                {
                // Handle breaking out of loop
                checkForBreakout( context );

                m_state = eIDLE;
                return Command_::eERROR_INVALID_ARGS;
                }
            break;
            

        case eCLAUSE_TRUE:
            if ( token == "ENDIF" )
                {
                endLevelTrue( context );

                // Handle breaking out of loop
                checkForBreakout( context );
                }
            else if ( token == "ELSE" )
                {
                context.enableFilter( *this );

                // Handle breaking out of loop
                if ( checkForBreakout( context ) )
                    {
                    endLevelTrue( context, false );
                    }
                }
            else if ( token == "ELIF" )
                {
                context.enableFilter( *this );

                // Handle breaking out of loop
                if ( checkForBreakout( context ) )
                    {
                    endLevelTrue( context, false );
                    }
                }
            else if ( token == "IF" )
                {
                if ( !checkForBreakout( context ) )
                    {
                    if ( !m_stack.push( m_state ) )
                        {
                        m_state = eIDLE;
                        return Command_::eERROR_INVALID_ARGS;
                        }
                    m_level++;
                    if ( (m_state=compare(context,tokens,vars)) == eCLAUSE_FALSE )
                        {
                        context.enableFilter( *this );
                        }
                    }
                else
                    {
                    // If I get here there is something messed up, i.e. shouldn't/can't happen
                    m_state = eIDLE;
                    return Command_::eERROR_INVALID_ARGS;
                    }
                }
            else
                {
                // Handle breaking out of loop
                checkForBreakout( context );

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


void Try::endLevelFalse( Cpl::TShell::Dac::Context_& context, bool enableFilter ) throw()
    {
    if ( m_level > m_stack.getNumItems() )
        {
        m_level--;
        if ( enableFilter )
            {
            context.enableFilter( *this );
            }
        }
    else
        {
        m_state = eIDLE;
        }
    }

void Try::endLevelTrue( Cpl::TShell::Dac::Context_& context, bool enableFilter ) throw()
    {
    m_state = popState();
    m_level--;
    if ( m_state != eIDLE && enableFilter )
        {
        context.enableFilter( *this );
        }
    }
               

///////////////////////////
bool Try::breaking( Loop* loopCmdInstance ) throw()
    {
    if ( m_state != eIDLE )
        {
        m_loopCmdPtr = loopCmdInstance;
        return true;
        }

    return false;
    }

bool Try::checkForBreakout( Cpl::TShell::Dac::Context_& context ) throw()
    {
    if ( m_loopCmdPtr )
        {
        context.enableFilter( *((Cpl::TShell::Dac::Command_*)m_loopCmdPtr) );
        m_loopCmdPtr = 0;
        return true;
        }

    return false;
    }


///////////////////////////
Try::State_T 
Try::compare( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, ActiveVariablesApi& vars ) throw()
    {
    if ( tokens.numParameters() < 5 )
        {
        return eCOMPARE_ERROR;
        }

    // Get operand values
    const char* op1 = getOperValue( tokens.getParameter(2), vars );
    const char* op2 = getOperValue( tokens.getParameter(4), vars );
    if ( !op1 || !op2 )
        {
        return eCOMPARE_ERROR;
        }

    return evaluate( op1, tokens.getParameter(3), op2 );
    }




Try::State_T Try::evaluate( const char* leftVal, const char* oper, const char* rightVal ) const throw()
    {
    int compareResult =  VariableApi::compare( leftVal, rightVal );

    if ( strcmp( oper, "=" ) == 0 || strcmp( oper, "==") == 0 )
        {
        return compareResult == 0? eCLAUSE_TRUE: eCLAUSE_FALSE;
        }
    else if ( strcmp( oper, "!=" ) == 0 )
        {
        return compareResult != 0? eCLAUSE_TRUE: eCLAUSE_FALSE;
        }
    else if ( strcmp( oper, "<" ) == 0 )
        {
        return compareResult < 0? eCLAUSE_TRUE: eCLAUSE_FALSE;
        }
    else if ( strcmp( oper, ">" ) == 0 )
        {
        return compareResult > 0? eCLAUSE_TRUE: eCLAUSE_FALSE;
        }
    else if ( strcmp( oper, "<=" ) == 0 )
        {
        return compareResult <= 0? eCLAUSE_TRUE: eCLAUSE_FALSE;
        }
    else if ( strcmp( oper, ">=" ) == 0 )
        {
        return compareResult >= 0? eCLAUSE_TRUE: eCLAUSE_FALSE;
        }
    else
        {
        return eCOMPARE_ERROR;
        }
    }


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



