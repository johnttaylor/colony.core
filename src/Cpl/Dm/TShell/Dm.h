#ifndef Cpl_Dm_TShell_Dm_h
#define Cpl_Dm_TShell_Dm_h
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
#include "Cpl/Dm/ModelDatabaseApi.h"


///
namespace Cpl {
///
namespace Dm {
///
namespace TShell {



/** This class implements a TShell command.  Note: Up to 10 different
    instance of this command can be instantiated - but each instance MUST
    have a different database number.  The database number specified by
    specifying the actual command name, e.g. 'dm0' is database number 0,
    'dm1' is database number 1, etc.
 */
class Dm : public Cpl::TShell::Cmd::Command
{
public:
    /// The command usage string
    static constexpr const char* usage = "dm ls [<filter>]\n" 
                                         "dm write {<mp-json>}\n" 
                                         "dm read <mpname>\n" 
                                         "dm touch <mpname>";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Lists, updates, and displays Model Points contained in the Model Database.\n" 
                                                "  When 'ls' is used a list of model point names is returned.  The <filter>\n" 
                                                "  argument will only list points that contain <filter>.  Updating a Model Point\n" 
                                                "  is done by specifying a JSON object. See the concrete class definition of the\n" 
                                                "  Model Point being updated for the JSON format.  When displaying a Model Point\n" 
                                                "  <mpname> is the string name of the Model Point instance to be displayed.";


protected:
    /// Model Point Database to access
    Cpl::Dm::ModelDatabaseApi& m_database;

    /// Dynamic 
public:
    /// See Cpl::TShell::Command                                                               `
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Dm( Cpl::Container::SList<Cpl::TShell::Command>&  commandList, 
        Cpl::Dm::ModelDatabaseApi&                  modelDatabase, 
        const char*                                 cmdNameAndDatabaseNumber="dm",
        Cpl::TShell::Security::Permission_T         minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
#endif  // end header latch
