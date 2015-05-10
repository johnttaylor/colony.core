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

