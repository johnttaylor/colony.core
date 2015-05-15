#ifndef Cpl_TShell_Dac_Cmd_Print_x_h
#define Cpl_TShell_Dac_Cmd_Print_x_h
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
/** @file */

#include "Cpl/TShell/Dac/Cmd/Command_.h"



///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {



/** This Private Namespace class is the base class for the printX commands
 */
class Print_: public Command_
{
public:
    /// Constructor
    Print_( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList, const char* verb ) throw();


public:
    /// See Cpl::TShell::Dac::Command_
    Cpl::TShell::Dac::Command_::Result_T execute( bool prependTimeStamp, Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
