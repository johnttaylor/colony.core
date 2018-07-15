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
/* 
   This file contains helper commands that are primarily used by the Scripting
   commands.  They are separated into a separate from Command.cpp so that
   an application that does not use Scripting commands does not 'pick-up'
   the object code for these functions
*/

#include "Command.h"
#include "Cpl/Text/strip.h"
#include "Cpl/TShell/Dac/Processor.h"



///
using namespace Cpl::TShell::Dac::Cmd;


#define ACTION_FIRST_   0
#define ACTION_AND_     1
#define ACTION_OR_      2



////////////////////////////////////////
Command::Result_T Command::expandText( const char* textToExpand, Cpl::Text::String& dst, Cpl::TShell::Dac::ActiveVariablesApi& vars ) throw()
    {
    // Generated expanded text
    const char* nextPtr;
    while( textToExpand && *textToExpand )
        {
        // Find the next embedded variable
        nextPtr = Cpl::Text::stripNotChars( textToExpand, OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_ );
        if ( *nextPtr )
            {
            // Save off the text BEFORE the found variable
            dst.appendTo( textToExpand, nextPtr - textToExpand );
            textToExpand = nextPtr + 1;

            // Escape the Escape character
            if ( *textToExpand == *OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_ )
                {
                dst += *OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_;
                textToExpand++;
                continue;
                }

            // Malformed textToExpand: <esc> character, but no variable
            if ( *textToExpand == '\0' )
                {
                return eERROR_INVALID_ARGS;
                }

            // Find the variable name
            nextPtr = Cpl::Text::stripNotChars( textToExpand, OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_ );
            if ( *nextPtr == '\0' )    
                {
                return eERROR_INVALID_ARGS;  // Malformed textToExpand: no trailing <esc>
                }

            // Look-up variable name
            Cpl::Text::FString<OPTION_CPL_TSHELL_DAC_VARIABLEAPI_NAME_SIZE> varname;
            varname.copyIn( textToExpand, nextPtr - textToExpand );
            VariableApi* varPtr;
            if ( (varPtr=vars.find(varname)) == 0 )
                {
                return eERROR_INVALID_ARGS;  // Malformed textToExpand: variable does not exist
                }

            // Substitute the variable's value
            dst += varPtr->getValue();
           
            // complete the variable sequence
            textToExpand = nextPtr + 1;
            }

        // Add the final/trailing non-expanded text (if there is any)            
        else
            {
            dst.appendTo( textToExpand, nextPtr - textToExpand );
            textToExpand = nextPtr;          
            }
        }

    return eSUCCESS;
    }



//////////////////////////////////////////////////////////
Command::CondResult_T Command::conditional( Cpl::TShell::Dac::Context_&           context, 
                                            Cpl::Text::Tokenizer::TextBlock&      tokens, 
                                            unsigned                              startingTokenIndex, 
                                            Cpl::TShell::Dac::ActiveVariablesApi& vars 
                                          ) throw()
    {
    CondResult_T result   = eTRUE;
    unsigned     numParms = tokens.numParameters();
    unsigned     action   = ACTION_FIRST_;

    for(;;)
        {
        // Trap missing operands/arguments
        if ( startingTokenIndex + 3 > numParms )
            {
            return eERROR;
            }


        // Get operand values
        Cpl::Text::String& op1     = context.getTokenBuffer();
        Cpl::Text::String& op2     = context.getTokenBuffer2();
        Result_T           result1 = expandText( tokens.getParameter(startingTokenIndex),   op1, vars );
        Result_T           result2 = expandText( tokens.getParameter(startingTokenIndex+2), op2, vars );
        if ( result1 != eSUCCESS || result2 != eSUCCESS )
            {
            return eERROR;
            }

        // Evaulate the statement
        CondResult_T temp = evaluate( op1, tokens.getParameter(startingTokenIndex+1), op2 );
        if ( action == ACTION_AND_ )
            {
            if ( result == eFALSE || temp == eFALSE )
                {
                return eFALSE;
                }
            }
        else if ( action == ACTION_OR_ )
            {
            if ( result == eTRUE || temp == eTRUE )
                {
                return eTRUE;
                }
            }
        result = temp;


        // Exit if no more compare statements
        startingTokenIndex += 3;
        if ( startingTokenIndex == numParms )
            {
            return result;
            }

        // Additional tokens - but the invalid number of additional tokens
        if ( startingTokenIndex + 4 > numParms )
            {
            return eERROR;
            }
        
        // Determine what action to take when evaulating the next compare statement
        const char* logical = tokens.getParameter( startingTokenIndex );
        if ( strcmp(logical,"AND") == 0 )
            {
            action = ACTION_AND_;
            }

        else if ( strcmp(logical,"OR") == 0 )
            {
            action = ACTION_OR_;
            }
        else
            {
            return eERROR;
            }

        // Advance to the start of the compare statement
        startingTokenIndex++;
        }
    }




Command::CondResult_T Command::evaluate( const char* leftVal, const char* oper, const char* rightVal ) throw()
    {
    int compareResult = VariableApi::compare( leftVal, rightVal );

    if ( strcmp( oper, "=" ) == 0 || strcmp( oper, "==") == 0 )
        {
        return compareResult == 0? eTRUE: eFALSE;
        }
    else if ( strcmp( oper, "!=") == 0 || strcmp( oper, "=!") == 0  )
        {
        return compareResult != 0? eTRUE: eFALSE;
        }
    else if ( strcmp( oper, "<" ) == 0 )
        {
        return compareResult < 0? eTRUE: eFALSE;
        }
    else if ( strcmp( oper, ">" ) == 0 )
        {
        return compareResult > 0? eTRUE: eFALSE;
        }
    else if ( strcmp( oper, "<=") == 0 || strcmp( oper, "=<") == 0  )
        {
        return compareResult <= 0? eTRUE: eFALSE;
        }
    else if ( strcmp( oper, ">=") == 0 || strcmp( oper, "=>") == 0  )
        {
        return compareResult >= 0? eTRUE: eFALSE;
        }
    else
        {
        return eERROR;
        }
    }
