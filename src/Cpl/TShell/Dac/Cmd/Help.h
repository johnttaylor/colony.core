#ifndef Cpl_TShell_Dac_Cmd_Help_h
#define Cpl_TShell_Dac_Cmd_Help_h
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

/** This class implements a DAC Shell command
 */
class Help: public Command_
{
protected:
    /// Verb
    static const char* m_verb  = "help";
    
    /// Usage
    static const char* m_usage = "help [full]"
    
    /// Details
    static const char* m_help  = "  Displays list of all supported commands and optionally their detailed help."
    
     
public:
    /// Constructor
    Help( Cpl::Container::Map<Command_>& commandList, Cpl::TShell::Dac::Context_& context )


public:
    /// See Cpl::TShell::Dac::Command_
    Cpl::TShell::Dac::Command_::Result_T execute( Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
