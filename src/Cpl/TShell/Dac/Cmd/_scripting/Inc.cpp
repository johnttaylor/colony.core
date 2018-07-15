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

#include "Cpl/TShell/Dac/Cmd/Inc.h"


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;

///////////////////////////
Inc::Inc( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Command(commandList, "inc")
    {
    }

Inc::Inc( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command(commandList, "inc", ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T Inc::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars  = context.getVariables();

    // Error checking
    if ( tokens.numParameters() > 3 )
        {
        return Command::eERROR_EXTRA_ARGS;
        }

    if ( tokens.numParameters() < 2 )
        {
        return Command::eERROR_MISSING_ARGS;
        }

    // Look-up variable name
    Cpl::Container::KeyLiteralString name( tokens.getParameter(1) );
    VariableApi*                     varPtr = vars.find( name );
    if ( !varPtr )
        {
        return Command::eERROR_INVALID_ARGS;
        }

    // Increment 
    const char* amount = "1";
    if ( tokens.numParameters() == 3 )
        {
        Cpl::Text::String& buf = context.getTokenBuffer();
        if ( expandText( tokens.getParameter(2), buf, vars ) == Command::eSUCCESS )
            {
            amount = buf;
            }
        else
            {
            return Command::eERROR_INVALID_ARGS;
            }
        }
    if ( !varPtr->add(amount) )
        {
        return Command::eERROR_FAILED;
        }

    // If I get here the command succeeded!
    return Command::eSUCCESS;
    }
