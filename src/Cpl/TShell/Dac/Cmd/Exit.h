#ifndef Cpl_TShell_Dac_Cmd_Exit_h
#define Cpl_TShell_Dac_Cmd_Exit_h
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

/* RULER
                                    "         1         2         3         4         5         6         7         8"
                                    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define CPLTSHELLDACMD_USAGE_EXIT_  "exit"
#define CPLTSHELLDACMD_DETAIL_EXIT_ "  Requests the DAC shell to exit.."


///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {

/** This Private Namespace class defines the interface for a DAC shell command.
 */
class Exit: public Command_
{
protected:
    /// See Cpl::TShell::Dac::Command_
    const char* getUsage() const throw()    { return CPLTSHELLDACMD_USAGE_EXIT_; }

    /// See Cpl::TShell::Dac::Command_
    const char* getHelp() const throw()     { return CPLTSHELLDACMD_DETAIL_EXIT_; }
    
     
public:
    /// Constructor
    Exit( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw();


public:
    /// See Cpl::TShell::Dac::Command_
    Cpl::TShell::Dac::Command_::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
