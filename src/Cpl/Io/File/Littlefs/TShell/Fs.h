#ifndef Cpl_Io_File_Littlefs_TShell_Fs_h
#define Cpl_Io_File_Littlefs_TShell_Fs_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/TShell/Cmd/Command.h"
#include "Cpl/Dm/ModelDatabaseApi.h"


///
namespace Cpl {
///
namespace Io {
///
namespace File {
///
namespace Littlefs {
///
namespace TShell {


/** This class implements a TShell command for various file system commands for
    a system that is using a Littlefs file system.
 */
class Fs : public Cpl::TShell::Cmd::Command
{
public:
    /// The command usage string
    static constexpr const char* verb = "fs";

    /// The command usage string
    static constexpr const char* usage = "fs ls <dir>\n" 
                                         "fs cat <textfile>\n" 
                                         "fs dump <binfile\n" 
                                         "fs vols";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Performs several file system actions.\n"
                                                "    ls   - Lists the content of directory. For a single volume system, use '/'\n"
                                                "           for the root directory.\n"
                                                "    cat  - Displays the content of the specified text file\n"
                                                "    dump - Displays the content of the specified binary file\n" 
                                                "    vols - Lists the volumes in the file system";


public:
    /// See Cpl::TShell::Command                                                               `
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Fs( Cpl::Container::Map<Cpl::TShell::Command>&  commandList, 
        unsigned                                    numVolumes,
        const char*                                 volumeRootPaths[],
        Cpl::TShell::Security::Permission_T         minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Root paths for the volumes
    const char** m_volPaths;    

    /// Number of volumes
    unsigned m_numVols;

};

}       // end namespaces
}
}
}
}
#endif  // end header latch
