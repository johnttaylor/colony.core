#ifndef Loki_Test_OnOff_Api_h_
#define Loki_Test_OnOff_Api_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "colony_config.h"
#include "Loki/Test/Test.h"
#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Timer.h"


/** Periodic interval, in milliseconds, used to generate on/off periods
 */
#ifndef OPTION_LOKI_TEST_ONOFF_INTERVAL_TIME_MS
#define OPTION_LOKI_TEST_ONOFF_INTERVAL_TIME_MS        1
#endif 

/** Default periodic interval, in milliseconds, used to for reporting status
 */
#ifndef OPTION_LOKI_TEST_ONOFF_REPORTING_TIME_MS
#define OPTION_LOKI_TEST_ONOFF_REPORTING_TIME_MS        (10*1000)   // 10sec
#endif 

 ///
namespace Loki {
///
namespace Test {
///
namespace OnOff {


/** This concrete class implements the OnOff Test.  The inputs/outputs
    are defined my the Model Point references used to construct the instance.

    Pausing the test set O to false.

    Features:  On/Off controller with optional hysteresis and optional minimum
               on/off times

    Inputs:
        Process Variable (PV).  Temperature in degrees Fahrenheit.
        Setpoint (SP)           Same units as PV.
        Optional on-hysteresis (HON). Default is 0. Same units as PV
        Optional off-hysteresis (HOFF). Default is 0. Same units as PV
        Optional minimum on time (MON).  Default is 0. Units:= milliseconds
        Optional minimum off time (MOFF).  Default is 0. Units:= milliseconds


    Outputs:
        On State (O)            Boolean output.

    Initial state:
        if ( PV > SP ) then O:= true else O:= false

    Logic:
        if ( PV > (SP + HON) AND deltaTime >= MON ) THEN O:= true
        if ( PV < (SP - HOFF) AND deltaTime >= MOFF ) THEN O:= false

 */
class Api : public Loki::Test::Test, public Cpl::System::Timer
{

public:
    /// My Test Name
    static const char* name() { return "OnOff"; }

public:
    /// Constructor
    Api( Cpl::Container::Map<TestApi>& testList,
         Cpl::Dm::MailboxServer&       myMbox,
         Cpl::Dm::Mp::Float&           processVarMp,
         Cpl::Dm::Mp::Bool&            valveOutputMp,
         Cpl::Dm::Mp::Bool&            pumpOuputMp);

public:
    /// See Loki::Test::TestApi
    const char* getHelp() const noexcept;

    /// See Loki::Test::TestApi
    void intialize() noexcept;

    /// See Loki::Test::TestApi
    void shutdown() noexcept;

    /// See Loki::Test::TestApi
    void start() noexcept;

    /// See Loki::Test::TestApi
    void stop() noexcept;

    /// See Loki::Test::TestApi
    void pause() noexcept;

    /// See Loki::Test::TestApi
    void resume() noexcept;

protected:
    /// Timer expired callback
    void expired( void ) noexcept;

public:
    /** Configures the test parameters.  This method can ONLY be called when the test is in the stopped/paused state
     */
    void configure( float       setpoint,
                    uint32_t    statusIntervalMs = OPTION_LOKI_TEST_ONOFF_REPORTING_TIME_MS,
                    float       onHysteresis     = 0.0F,
                    float       offHysteresis    = 0.0F,
                    uint32_t    minOnMs          = 0,
                    uint32_t    minOffMs         = 0 );

    /// Returns the current test parameters
    void getConfig( float&       setpoint,
                    uint32_t&    statusIntervalMs,
                    float&       onHysteresis,
                    float&       offHysteresis,
                    uint32_t&    minOnMs,
                    uint32_t&    minOffMs ) const noexcept;
protected:
    /// Input
    Cpl::Dm::Mp::Float& m_pv;

    /// Output that controls the 3way valve
    Cpl::Dm::Mp::Bool&  m_valveOutput;

    /// Output that controls the water pump motor
    Cpl::Dm::Mp::Bool&  m_pumpOutput;

    /// Setpoint
    float               m_setpoint;

    /// On hysteresis
    float               m_onHyseteresis;

    /// On hysteresis
    float               m_offHyseteresis;

    /// Minimum On time in milliseconds
    uint32_t            m_minOnMs;

    /// Minimum Off time in milliseconds
    uint32_t            m_minOffMs;

    /// Time marker
    uint32_t            m_marker;

    /// Interval timing for displaying status
    uint32_t            m_statusIntervalMs;

    /// Time marker when transitioning to paused
    uint32_t            m_markerPaused;

    /// Time marker outputting status
    uint32_t            m_markerStatus;

    /// On/Off state
    bool                m_onState;

    /// State of the value when paused
    bool                m_valvePausedState;
};

};      // end namespaces
};
};
#endif  // end header latch
