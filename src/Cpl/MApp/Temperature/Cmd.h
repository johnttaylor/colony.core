#ifndef Loki_Test_OnOff_Cmd_h
#define Loki_Test_OnOff_Cmd_h
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/TShell/Cmd/Command.h"
#include "Loki/Test/OnOff/Api.h"
#include "Loki/Test/Api.h"

///
namespace Loki {
///
namespace Test {
///
namespace OnOff {

/** Used for low-level testing of the Motor.
 */
class Cmd : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "onoff";

    /// The command usage string
    static constexpr const char* usage = "onoff\n"
                                         "onoff config <setpt> [<rptms> <hon> <hoff> <mon> <moff>]\n"
                                         "onoff start|stop\n"
                                         "onoff pause|resume";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  On/Off Controller. The pump motor is ON while the test is running and the "
                                                "  value is cycled to control water temperature. The configuration arguments are:\n"
                                                "    <setpt>   temperature setpoint is 'F\n"
                                                "    <rptms>   the reporting interval (in millisecond) for the current state\n"
                                                "    <hon>     number of degrees 'F for ON hysteresis (default is 0).\n"
                                                "    <hoff>    number of degrees 'F for OFF hysteresis (default is 0).\n"
                                                "    <mon>     enforced minimum on time in milliseconds (default is 0).\n"
                                                "    <moff>    enforced minimum off time in milliseconds (default is 0).";

public:
    /// See Cpl::TShell::Command                                                               `
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Cmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Loki::Test::Api& testManager, Api& m_testHandle ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Handle to the Test manager
    Loki::Test::Api&    m_testManager;

    /// Handle to a OnOff test instance
    Api&                m_testHdl;
};

};      // end namespaces
};
};
#endif  // end header latch
