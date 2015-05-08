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

#include "Cpl/TShell/Dac/Cmd/Print_.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/format.h"
#include "Cpl/System/ElaspedTime.h"


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;


static void initializeOuttext_( Cpl::Text::String& outtext, bool prependTimeStamp );


///////////////////////////
Print_::Print_( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList, const char* verb ) throw()
:Command_(commandList, verb)
    {
    }

///////////////////////////
Cpl::TShell::Dac::Command_::Result_T Print_::execute( bool prependTimeStamp, Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars   = context.getVariables();
    Cpl::Text::String& outtext = context.getTokenBuffer();
    const char*         etext;

    // Error checking
    if ( tokens.numParameters() > 3 )
        {
        return Command_::eERROR_EXTRA_ARGS;
        }

    if ( tokens.numParameters() == 1 && !prependTimeStamp )
        {
        return Command_::eERROR_MISSING_ARGS;
        }


    // Set the default escape character
    char esc[2];
    esc[0] = *OPTION_CPL_TSHELL_DAC_CMD_PRINT_ESCAPE_CHAR;
    esc[1] = '\0';

    // Trap non-default escape character
    if ( tokens.numParameters() == 3 )
        {
        esc[0] = *(tokens.getParameter(1));
        etext  = tokens.getParameter(2);
        }
    else
        {
        etext = tokens.getParameter(1);
        }


    // Generated expanded text
    char* nextPtr;
    initializeOuttext_( outtext, prependTimeStamp );
    while( etext && *etext )
        {
        // Find the next embedded variable
        nextPtr = (char*) Cpl::Text::stripNotChars( etext, esc );
        if ( *nextPtr )
            {
            // Save off the text BEFORE the found variable
            outtext.appendTo( etext, nextPtr - etext );
            etext = nextPtr + 1;

            // Malformed etext: <esc> character, but no variable
            if ( *etext == '\0' )
                {
                return Command_::eERROR_INVALID_ARGS;
                }

            // Find the variable name
            nextPtr = (char*) Cpl::Text::stripNotChars( etext, esc );
            if ( *nextPtr == '\0' )    
                {
                return Command_::eERROR_INVALID_ARGS;  // Malformed etext: no trailing <esc>
                }

            // Look-up variable name
            *nextPtr = '\0'; // null terminate variable name
            Cpl::Container::KeyLiteralString varname( etext );
            VariableApi*                     varPtr;
            if ( (varPtr=vars.find(varname)) == 0 )
                {
                return Command_::eERROR_INVALID_ARGS;  // Malformed etext: variable does not exist
                }

            // Substitute the variable's value
            outtext += varPtr->getValue();
           
            // complete the variable sequence
            etext = nextPtr + 1;
            }

        // Add the final/trailing non-expanded text (if there is any)            
        else
            {
            outtext.appendTo( etext, nextPtr - etext );
            etext = nextPtr;          
            }
        }
        

    // Output the final expanded text
    return context.outputMessage( outfd, outtext )? Command_::eSUCCESS: Command_::eERROR_IO;
    }



void initializeOuttext_( Cpl::Text::String& outtext, bool prependTimeStamp )
    {
    // NO timestamp
    if ( !prependTimeStamp )
        {
        outtext.clear();
        }

    // Generate the current timestamp
    else
        {
        Cpl::Text::formatPrecisionTimeStamp( outtext, Cpl::System::ElaspedTime::precision() );
        outtext += "  ";
        }
    }

