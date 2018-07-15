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

#include "Cpl/TShell/Dac/Cmd/TPrint.h"


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;



///////////////////////////
TPrint::TPrint( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Print_(commandList, "tprint")
    {
    }

TPrint::TPrint( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Print_(commandList, "tprint", ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T TPrint::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    return Print_::execute( true, context, tokens, rawInputString, outfd );
    }
