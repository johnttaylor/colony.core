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

#include "Cpl/TShell/Dac/Cmd/Exit.h"


///
using namespace Cpl::TShell::Dac::Cmd;

///////////////////////////
Exit::Exit( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw()
:Command_(commandList, "exit")
    {
    }

///////////////////////////
Cpl::TShell::Dac::Command_::Result_T Exit::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    // Error checking
    if ( tokens.numParameters() > 1 )
        {
        return Cpl::TShell::Dac::Command_::eERROR_EXTRA_ARGS;
        }

    // Request the Command Processor to stop
    context.requestStop();
    return Cpl::TShell::Dac::Command_::eSUCCESS;
    }

