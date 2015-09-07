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

#include "TQuery.h"

///
using namespace Rte::TShell::Dac::Cmd;


///////////////////////////
TQuery::TQuery( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList ) throw()
:Command( commandList, "tquery", modelPointList )
    {
    }

TQuery::TQuery( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command( commandList, "tquery", modelPointList, ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance )
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T TQuery::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    // Display Points
    Cpl::TShell::Dac::Command::Result_T result = listPoints(context, tokens);
    if ( result != Command::eERROR_FAILED )
        {
        return result;
        }


    // If I get here the command failed!
    return Command::eERROR_FAILED;
    }
