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

#include "Cpl/TShell/Dac/Cmd/Exe.h"


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;

///////////////////////////
Exe::Exe( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Command(commandList, "exe")
    {
    }


Exe::Exe( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command(commandList, "exe", ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T Exe::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars  = context.getVariables();
    Cpl::Text::String&  etext = context.getTokenBuffer();

    // Error checking
    if ( tokens.numParameters() > 2 )
        {
        return Command::eERROR_EXTRA_ARGS;
        }
    if ( tokens.numParameters() < 2 )
        {
        return Command::eERROR_MISSING_ARGS;
        }

    // Expand the command string
    Command::Result_T result = expandText( tokens.getParameter(1), etext, vars );
    if ( result != Command::eSUCCESS )
        {
        return result;
        }


    // Execute the command
    return context.executeCommand( etext, outfd );
    }
