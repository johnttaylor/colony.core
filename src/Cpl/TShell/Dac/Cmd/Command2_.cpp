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

#include "Command_.h"
#include "Cpl/Text/strip.h"
#include "Cpl/TShell/Dac/Processor.h"


/* 

   This file contains helper commands that are primarly used by the Scripting
   commands.  They are seperated into a seperate from Command_.cpp so that
   an application that does not use Scripting commands does not 'pick-up'
   the object code for these functions

*/

///
using namespace Cpl::TShell::Dac::Cmd;




////////////////////////////////////////
Command_::Result_T Command_::expandText( const char* textToExpand, Cpl::Text::String& dst, char escChar, Cpl::TShell::Dac::ActiveVariablesApi& vars ) throw()
    {
    // Convert the escape character to a null terminated string
    char esc[2];
    esc[0] = escChar;
    esc[1] = '\0';

    // Generated expanded text
    char* nextPtr;
    while( textToExpand && *textToExpand )
        {
        // Find the next embedded variable
        nextPtr = (char*) Cpl::Text::stripNotChars( textToExpand, esc );
        if ( *nextPtr )
            {
            // Save off the text BEFORE the found variable
            dst.appendTo( textToExpand, nextPtr - textToExpand );
            textToExpand = nextPtr + 1;

            // Malformed textToExpand: <esc> character, but no variable
            if ( *textToExpand == '\0' )
                {
                return eERROR_INVALID_ARGS;
                }

            // Find the variable name
            nextPtr = (char*) Cpl::Text::stripNotChars( textToExpand, esc );
            if ( *nextPtr == '\0' )    
                {
                return eERROR_INVALID_ARGS;  // Malformed textToExpand: no trailing <esc>
                }

            // Look-up variable name
            *nextPtr = '\0'; // null terminate variable name
            Cpl::Container::KeyLiteralString varname( textToExpand );
            VariableApi*                     varPtr;
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


const char* Command_::getOperValue( const char* oper, Cpl::TShell::Dac::ActiveVariablesApi& vars ) throw()
    {
    if ( *oper == '#' )
        {
        return oper + 1;
        }
    else 
        {
        Cpl::Container::KeyLiteralString name( oper );
        VariableApi*                     varPtr = vars.find( name );
        if ( varPtr )
            {
            return varPtr->getValue();
            }
        }

    // If I get then bad syntax OR non-existant variable
    return 0;
    }



#define ACTION_FIRST_   0
#define ACTION_AND_     1
#define ACTION_OR_      2

//////////////////////////////////////////////////////////
Command_::CondResult_T Command_::conditional( Cpl::TShell::Dac::Context_&           context, 
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
        const char* op1 = getOperValue( tokens.getParameter(startingTokenIndex),   vars );
        const char* op2 = getOperValue( tokens.getParameter(startingTokenIndex+2), vars );
        if ( !op1 || !op2 )
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




Command_::CondResult_T Command_::evaluate( const char* leftVal, const char* oper, const char* rightVal ) throw()
    {
    int compareResult =  VariableApi::compare( leftVal, rightVal );

    if ( strcmp( oper, "=" ) == 0 || strcmp( oper, "==") == 0 )
        {
        return compareResult == 0? eTRUE: eFALSE;
        }
    else if ( strcmp( oper, "!=" ) == 0 )
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
    else if ( strcmp( oper, "<=" ) == 0 )
        {
        return compareResult <= 0? eTRUE: eFALSE;
        }
    else if ( strcmp( oper, ">=" ) == 0 )
        {
        return compareResult >= 0? eTRUE: eFALSE;
        }
    else
        {
        return eERROR;
        }
    }
