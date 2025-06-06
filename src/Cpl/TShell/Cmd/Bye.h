#ifndef Cpl_TShell_Cmd_Bye_h
#define Cpl_TShell_Cmd_Bye_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "Cpl/TShell/Cmd/Command.h"

///
namespace Cpl {
///
namespace TShell {
///
namespace Cmd {

/** This class implements a  Shell command
 */
class Bye : public Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "bye";

    /// The command usage string
    static constexpr const char* usage = "bye [app [<exitcode>]]";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Requests the  shell to exit. If the optional argument 'app' is specified\n" 
                                                "  then the application is exited with the specified <exitcode>. The default\n" 
                                                "  <exitcode> is '0'.";


protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Bye( Cpl::Container::SList<Cpl::TShell::Command>& commandList,
         Security::Permission_T                     minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
#endif  // end header latch
