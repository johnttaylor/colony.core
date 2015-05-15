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

#include "Cpl/TShell/Dac/Cmd/Exe.h"


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;

///////////////////////////
Exe::Exe( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw()
:Command_(commandList, "exe")
    {
    }

///////////////////////////
Cpl::TShell::Dac::Command_::Result_T Exe::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars  = context.getVariables();
    Cpl::Text::String&  etext = context.getTokenBuffer();
    const char*         cmdstring;

    // Error checking
    if ( tokens.numParameters() > 3 )
        {
        return Command_::eERROR_EXTRA_ARGS;
        }
    if ( tokens.numParameters() < 2 )
        {
        return Command_::eERROR_MISSING_ARGS;
        }

    // Set the default escape character
    char esc = *OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_;

    // Trap non-default escape character
    if ( tokens.numParameters() == 3 )
        {
        esc       = *(tokens.getParameter(1));
        cmdstring = tokens.getParameter(2);
        }
    else
        {
        cmdstring = tokens.getParameter(1);
        }

    // Expand the command string
    etext.clear();
    Command_::Result_T result = expandText( cmdstring, etext, esc, vars );
    if ( result != Command_::eSUCCESS )
        {
        return result;
        }


    // Execute the command
    return context.executeCommand( etext, outfd );
    }
