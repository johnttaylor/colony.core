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

#include "Cpl/TShell/Dac/Cmd/ToDec.h"
#include "Cpl/Text/atob.h"
#include <string.h>



///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;

///////////////////////////
ToDec::ToDec( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Command(commandList, "todec")
    {
    }

ToDec::ToDec( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command(commandList, "todec", ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T ToDec::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars     = context.getVariables();
    unsigned            numParms = tokens.numParameters();


    // Error checking
    if ( numParms < 3 )
        {
        return Command::eERROR_MISSING_ARGS;
        }
    if ( numParms > 4 )
        {
        return Command::eERROR_EXTRA_ARGS;
        }


    // Expand the text that will be converted
    Cpl::Text::String& etext  = context.getTokenBuffer();
    Command::Result_T  result = expandText( tokens.getParameter(1), etext, vars );
    if ( result != Command::eSUCCESS )
        {
        return result;
        }

    // Get base argument (if there is one)
    int base = 16;
    if ( numParms == 4 )
        {
        if ( !Cpl::Text::a2i(base, tokens.getParameter(3)) )
            {
            return Command::eERROR_INVALID_ARGS;
            }
        }

    // Perform the conversion
    unsigned long temp;
    if ( !Cpl::Text::a2ul(temp, etext, base) )
        {
        return Command::eERROR_INVALID_ARGS;
        }


    // Store the result
    Cpl::Container::KeyLiteralString name( tokens.getParameter(2) );
    VariableApi*                     varPtr = vars.find( name );
    if ( !varPtr )
        {
        return Command::eERROR_INVALID_ARGS;
        }
    varPtr->setValue( temp );


    // If I get here, everything worked!
    return Command::eSUCCESS;
    }
