#ifndef Cpl_TShell_Dac_Cmd_Inc_h
#define Cpl_TShell_Dac_Cmd_Inc_h
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
#define CPLTSHELLDACMD_USAGE_INC_     "inc <var> [<amount>]" 
#ifndef CPLTSHELLDACMD_DETAIL_INC_
#define CPLTSHELLDACMD_DETAIL_INC_    "  Increments the contents of <var> by 1.  An optional <amount> to increment the\n" \
                                      "  variable can be provided. Note: <amount> can be a literal (prefixed with '#')\n" \
                                      "  or a variable; and the value can be negative.  If the contents of <var> is not\n" \
                                      "  numeric or <amount> is not numeric than contents of <var> remains unchanged\n" \
                                      "  and an error is returned."


#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue

///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class Inc: public Command
{
public:
    /// See Cpl::TShell::Dac::Command
    const char* getUsage() const throw()    { return CPLTSHELLDACMD_USAGE_INC_; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const throw()    { return CPLTSHELLDACMD_DETAIL_INC_; }
    
     
public:
    /// Constructor
    Inc( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw();


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();
            


};

};      // end namespaces
};
};
};
#endif  // end header latch
