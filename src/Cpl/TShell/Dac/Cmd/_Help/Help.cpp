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

#include "Cpl/TShell/Dac/Cmd/Help.h"


///
using namespace Cpl::TShell::Dac::Cmd;

///////////////////////////
Help::Help( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw()
:Command_(commandList, "help")
    {
    }

///////////////////////////
Cpl::TShell::Dac::Command_::Result_T Help::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    // Error checking
    if ( tokens.numParameters() > 2 )
        {
        return Cpl::TShell::Dac::Command_::eERROR_EXTRA_ARGS;
        }

    // List the commands
    Cpl::Container::Map<Cpl::TShell::Dac::Command_>& cmdList = context.getCommands();
    Cpl::TShell::Dac::Command_*                      cmdPtr  = cmdList.first();
    while( cmdPtr )
        {
        context.outputMessage( outfd, cmdPtr->getUsage() );
        if ( tokens.numParameters() == 2 )
            {
            const char* details = cmdPtr->getHelp();
            if ( details )
                {
                context.outputMessage( outfd, details );
                context.outputMessage( outfd, " " );
                }
            }

        cmdPtr = cmdList.next( *cmdPtr );
        }

    return Cpl::TShell::Dac::Command_::eSUCCESS;
    }

