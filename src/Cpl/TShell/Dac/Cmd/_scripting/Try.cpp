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

            if ( (m_state = compare(context,tokens,vars)) == eCLAUSE_FALSE )
                {
                context.enableFilter( *this );
                }
            break;
            

        case eCLAUSE_FALSE:
            if ( token == "ENDIF" )
                {
                m_state = eIDLE;
                }
            else if ( token == "ELSE" )
                {
                m_state = eCLAUSE_TRUE;
                }
            else if ( token == "ELIF" )
                {
                if ( (m_state = compare(context,tokens,vars)) == eCLAUSE_FALSE ) 
                    {
                    context.enableFilter( *this );
                    }
                }
            else
                {
                return Command_::eERROR_INVALID_ARGS;
                }
            break;
            
               
        case eCLAUSE_TRUE:
            if ( token == "ENDIF" )
                {
                m_state = eIDLE;
                }
            else if ( token == "ELSE" )
                {
                context.enableFilter( *this );
                }
            else if ( token == "ELIF" )
                {
                context.enableFilter( *this );
                }
            else
                {
                return Command_::eERROR_INVALID_ARGS;
                }
            break;


        case eCOMPARE_ERROR:
            return Command_::eERROR_FAILED;
        }


    // If I get the command succeeded!
    return Command_::eSUCCESS;
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