#ifndef Cpl_TShell_Dac_Cmd_Command_x_h_
#define Cpl_TShell_Dac_Cmd_Command_x_h_
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

#include "Cpl/TShell/Dac/Command_.h"
#include "Cpl/Container/Map.h"


///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {

/** This partially concrete Private Namespace class implements infrastructure
    and/or common functionality for a DAC Shell command
 */
class Command_: public Cpl::TShell::Dac::Command_
{
private:
    /// Verb
    const char* m_verb;

    /// Usage string
    const char* m_usage;

    /// Detailed help
    const char* m_help;


protected:
    /// Constructor
    Command_( Cpl::Container::Map<Command_>& commandList, const char* verb, const char* usage, const char* help=0 ) throw();


public:
    /// See Cpl::TShell::Dac::Command_
    const char* getVerb() const throw();

    /// See Cpl::TShell::Dac::Command_
    const char* getUsage() const throw();

    /// See Cpl::TShell::Dac::Command_
    const char* getHelp() const throw();
};

};      // end namespaces
};
};
};
#endif  // end header latch
