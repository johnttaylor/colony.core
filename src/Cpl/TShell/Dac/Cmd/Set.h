#ifndef Cpl_TShell_Dac_Cmd_Set_h
#define Cpl_TShell_Dac_Cmd_Set_h
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

#include "colony_config.h"
#include "Cpl/TShell/Dac/Cmd/Command.h"


/* RULER
                                    "         1         2         3         4         5         6         7         8"
                                    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define CPLTSHELLDACMD_USAGE_SET_   "set [<varname> [<etext>]]"
#ifndef CPLTSHELLDACMD_DETAIL_SET_
#define CPLTSHELLDACMD_DETAIL_SET_  "  Updates the contents of the variable <varname> to the expanded <etext>. If\n" \
                                    "  only the <varname> argument is provided, then the variable is removed from the\n" \
                                    "  active variable list. If no arguments are provided, then a list of all active\n" \
                                    "  variables (and their current value) is generated. <etext> is literal text with\n" \
                                    "  an optional escape sequences that embeds a varaible's value into the string.\n" \
                                    "  The escape sequnce is: " OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_ "<varname>" OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_ ".  <etext> is THE mechanism for referencing\n" \
                                    "  a variable's value."
#endif

///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class Set: public Command
{
public:
    /// See Cpl::TShell::Dac::Command
    const char* getUsage() const throw()    { return CPLTSHELLDACMD_USAGE_SET_; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const throw()    { return CPLTSHELLDACMD_DETAIL_SET_; }
    
     
public:
    /// Constructor
    Set( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw();


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
