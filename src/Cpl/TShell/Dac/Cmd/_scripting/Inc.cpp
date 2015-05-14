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

#include "Cpl/TShell/Dac/Cmd/Inc.h"


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;

///////////////////////////
Inc::Inc( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw()
:Command_(commandList, "inc")
    {
    }

///////////////////////////
Cpl::TShell::Dac::Command_::Result_T Inc::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars  = context.getVariables();

    // Error checking
    if ( tokens.numParameters() > 3 )
        {
        return Command_::eERROR_EXTRA_ARGS;
        }

    if ( tokens.numParameters() < 2 )
        {
        return Command_::eERROR_MISSING_ARGS;
        }

    // Look-up variable name
    Cpl::Container::KeyLiteralString name( tokens.getParameter(1) );
    VariableApi*                     varPtr = vars.find( name );
    if ( !varPtr )
        {
        return Command_::eERROR_INVALID_ARGS;
        }

    // Increment 
    const char* amount = "1";
    if ( tokens.numParameters() == 3 )
        {
        amount = getOperValue( tokens.getParameter(2), vars );
        }
    if ( !varPtr->add(amount) )
        {
        return Command_::eERROR_FAILED;
        }

    // If I get here the command succeeded!
    return Command_::eSUCCESS;
    }
