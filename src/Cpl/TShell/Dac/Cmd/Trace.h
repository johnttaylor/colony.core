#ifndef Cpl_TShell_Dac_Cmd_Trace_h
#define Cpl_TShell_Dac_Cmd_Trace_h
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

#include "colony_config.h"
#include "Cpl/TShell/Dac/Cmd/Command.h"


/** Usage
                                    "         1         2         3         4         5         6         7         8"
                                    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define CPLTSHELLDACMD_USAGE_TRACE_   "trace [on|off]\n" \
                                      "trace section (on|off) <sect1> [<sect2>]...\n" \
                                      "trace threadfilters [<threadname1> [<threadname2>]]...\n" \
                                      "trace level (none|brief|info|verbose|max)\n" \
                                      "trace here|revert"

/// Detailed Help text
#ifndef CPLTSHELLDACMD_DETAIL_TRACE_
#define CPLTSHELLDACMD_DETAIL_TRACE_  "  Enables/Disables the Cpl::System::Trace engine and manages the section',\n" \
                                      "  information level, and thread filter options.  See the Cpl::System::Trace\n" \
                                      "  interface for details on how the trace engine works."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class Trace: public Command
{
public:
    /// See Cpl::TShell::Dac::Command
    const char* getUsage() const noexcept   { return CPLTSHELLDACMD_USAGE_TRACE_; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const noexcept    { return CPLTSHELLDACMD_DETAIL_TRACE_; }
    
     
public:
    /// Constructor
    Trace( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) noexcept;

    /// Constructor.  Used to create a static instance of the command
    Trace( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) noexcept;


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
};
#endif  // end header latch
