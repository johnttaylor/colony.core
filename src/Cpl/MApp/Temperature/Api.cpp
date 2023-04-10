/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include <stdlib.h>

using namespace Loki::Test::OnOff;

static uint32_t setStatusIntervalTime( uint32_t now, uint32_t intervalTime )
{
    // Round down to the nearest interval boundary
    return ( now / intervalTime ) * intervalTime;
}

/////////////////////////////////////////////////////
Api::Api( Cpl::Container::Map<TestApi>& testList,
          Cpl::Dm::MailboxServer&       myMbox,
          Cpl::Dm::Mp::Float&           processVarMp,
          Cpl::Dm::Mp::Bool&            valveOutputMp,
          Cpl::Dm::Mp::Bool&            pumpOuputMp )
    : Loki::Test::Test( testList, name() )
    , Cpl::System::Timer( myMbox )
    , m_pv( processVarMp )
    , m_valveOutput( valveOutputMp )
    , m_pumpOutput( pumpOuputMp )
    , m_setpoint( 75.0F )
    , m_onHyseteresis( 0.0F )
    , m_offHyseteresis( 0.0F )
    , m_minOnMs( 0 )
    , m_minOffMs( 0 )
    , m_statusIntervalMs( OPTION_LOKI_TEST_ONOFF_REPORTING_TIME_MS )
{
}

void Api::configure( float       setpoint,
                     uint32_t    statusIntervalMs,
                     float       onHysteresis,
                     float       offHysteresis,
                     uint32_t    minOnMs,
                     uint32_t    minOffMs )
{
    m_setpoint         = setpoint;
    m_statusIntervalMs = statusIntervalMs;
    m_onHyseteresis    = onHysteresis;
    m_offHyseteresis   = offHysteresis;
    m_minOnMs          = minOnMs;
    m_minOffMs         = minOffMs;
}

void Api::getConfig( float&       setpoint,
                     uint32_t&    statusIntervalMs,
                     float&       onHysteresis,
                     float&       offHysteresis,
                     uint32_t&    minOnMs,
                     uint32_t&    minOffMs ) const noexcept
{
    setpoint         = m_setpoint;
    statusIntervalMs = m_statusIntervalMs;
    onHysteresis     = m_onHyseteresis;
    offHysteresis    = m_offHyseteresis;
    minOnMs          = m_minOnMs;
    minOffMs         = m_minOffMs;
}

const char* Api::getHelp() const noexcept
{
    return "No help";
}

void Api::intialize() noexcept
{
}

void Api::shutdown() noexcept
{
    // Ensure that everything is stopped
    Cpl::System::Timer::stop();
    m_valveOutput.write( false );
}

void Api::start() noexcept
{
    CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "Configuration: setpt=%g, hon=%g, hoff=%g, mon=%lu, moff=%lu",
                          m_setpoint,
                          m_onHyseteresis,
                          m_offHyseteresis,
                          m_minOnMs,
                          m_minOffMs ) );

    // Housekeeping
    m_started          = true;
    m_running          = true;
    m_valvePausedState = false;

    // Set initial set
    float pv     = 0.0F;
    bool  output = false;
    if ( m_pv.read( pv ) )
    {
        if ( pv >= m_setpoint )
        {
            output = true;
        }
        else
        {
            output = false;
        }
    }
    m_valveOutput.write( output );
    m_pumpOutput.write( true );
    m_marker       = Cpl::System::ElapsedTime::milliseconds();
    m_markerStatus = setStatusIntervalTime( m_marker, m_statusIntervalMs );

    // Display status
    CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "OnOff Status: pv=%g, setpt=%g, output=%s",
                          pv, m_setpoint, output ? "ON" : "off" ) );

    // Start my polling timer
    Cpl::System::Timer::start( OPTION_LOKI_TEST_ONOFF_INTERVAL_TIME_MS );
}

void Api::stop() noexcept
{
    // Stop the algorithm
    m_pumpOutput.write( false );
    m_valveOutput.write( false );
    Cpl::System::Timer::stop();

    // Housekeeping
    m_started = false;
    m_running = false;
}


void Api::pause() noexcept
{
    m_running = false;
    m_valveOutput.read( m_valvePausedState );
    m_pumpOutput.write( false );
    m_valveOutput.write( false );
    m_markerPaused = Cpl::System::ElapsedTime::milliseconds();
}


void Api::Api::resume() noexcept
{
    m_running = true;
    CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "Configuration: setpt=%g, hon=%g, hoff=%g, mon=%lu, moff=%lu",
                          m_setpoint,
                          m_onHyseteresis,
                          m_offHyseteresis,
                          m_minOnMs,
                          m_minOffMs ) );

    // Resume where we left off (adjust my timing to account for the paused-duration)
    uint32_t pausedDuration = Cpl::System::ElapsedTime::deltaMilliseconds( m_markerPaused );
    m_marker += pausedDuration;
    m_pumpOutput.write( true );
    m_valveOutput.write( m_valvePausedState );
    expired();
}

void Api::expired() noexcept
{
    if ( m_running )
    {
        // Housekeeping
        uint32_t now          = Cpl::System::ElapsedTime::milliseconds();
        uint32_t deltaCycleMs = Cpl::System::ElapsedTime::deltaMilliseconds( m_marker, now );
        float pv              = 0.0F;
        bool  currentOutput   = false;
        m_pv.read( pv );
        m_valveOutput.read( currentOutput );

        //Logic:
        //     if ( PV > (SP + HON) AND deltaTime >= MON ) THEN O:= true
        //     if ( PV < (SP - HOFF) AND deltaTime >= MOFF ) THEN O:= false

        // Currently Off
        if ( !currentOutput )
        {
            // Transition to ON
            if ( pv > ( m_setpoint + m_onHyseteresis ) && deltaCycleMs >= m_minOffMs )
            {
                currentOutput = true;
                m_valveOutput.write( currentOutput );
                m_marker = now;
                CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "off->ON: pv=%g, setpt=%g, output=%s",
                                      pv, m_setpoint, currentOutput ? "ON" : "off" ) );
            }
        }

        // Currently ON
        else
        {
            // Transition to OFF
            if ( Cpl::Math::areFloatsEqual( pv, 0.0F ) ||   // Trap invalid MP input
                 ( pv < ( m_setpoint - m_offHyseteresis ) && deltaCycleMs >= m_minOnMs ) )
            {
                currentOutput = false;
                m_valveOutput.write( currentOutput );
                m_marker = now;
                CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "ON->off: pv=%g, setpt=%g, output=%s",
                                      pv, m_setpoint, currentOutput ? "ON" : "off" ) );
            }
        }

        // Check for status interval
        uint32_t deltaStatus = Cpl::System::ElapsedTime::deltaMilliseconds( m_markerStatus, now );
        if ( deltaStatus > m_statusIntervalMs )
        {
            // set my time marker and account for any delays (i.e. trying to be monotonic)
            m_markerStatus = setStatusIntervalTime( now, m_statusIntervalMs );

                    // Display status
            CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "Status:  pv=%g, setpt=%g, output=%s",
                                  pv, m_setpoint, currentOutput ? "ON" : "off" ) );
        }
    }

    // Restart my timer
    Cpl::System::Timer::start( OPTION_LOKI_TEST_ONOFF_INTERVAL_TIME_MS );
}

