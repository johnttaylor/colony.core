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


static void outputCmdHelp_( Cpl::TShell::Dac::Context_& context, Cpl::TShell::Dac::Command_& cmd, bool& io, bool includeDetails );
static void outputLongText_( Cpl::TShell::Dac::Context_& context, bool& io, const char* text );


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
        return Command_::eERROR_EXTRA_ARGS;
        }

    // Housekeeping
    Cpl::Container::Map<Cpl::TShell::Dac::Command_>&  cmdList = context.getCommands();
    bool                                              io      = true;
    Cpl::TShell::Dac::Command_*                       cmdPtr;
    Cpl::Container::KeyLiteralString                  verb( tokens.getParameter(1) );

    // Command specific help
    if ( (cmdPtr=cmdList.find(verb)) )
        {
        outputCmdHelp_( context, *cmdPtr, io, true );
        }

    // List the commands
    else
        {
        cmdPtr = cmdList.first();

        while( cmdPtr && io == true )
            {
            outputCmdHelp_( context, *cmdPtr, io, tokens.numParameters() == 2 );
            cmdPtr = cmdList.next( *cmdPtr );
            }
        }

    return io? Command_::eSUCCESS: Command_::eERROR_IO;
    }



void outputCmdHelp_( Cpl::TShell::Dac::Context_& context, Cpl::TShell::Dac::Command_& cmd, bool& io, bool includeDetails )
    {
    outputLongText_( context, io, cmd.getUsage() );
    if ( includeDetails )
        {
        const char* details = cmd.getHelp();
        if ( details && *details != '\0' )
            {
            outputLongText_( context, io, details );
            io &= context.writeFrame( " " );
            }
        }
    }


void outputLongText_( Cpl::TShell::Dac::Context_& context, bool& io, const char* text )
    {
    const char* ptr = text;
    while( *ptr )
        {
        if ( *ptr == '\n' )
            {
            io &= context.writeFrame( text, ptr - text );
            text = ++ptr;
            }
        else
            {
            ptr++;
            }
        }

    size_t numBytes = ptr - text;
    if ( numBytes )
        {
        io &= context.writeFrame( text, numBytes );
        }
    }
