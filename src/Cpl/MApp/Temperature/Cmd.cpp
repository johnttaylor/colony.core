/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/

#include "Cmd.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/Text/atob.h"
#include "Cpl/System/Api.h"
#include "Loki/mp/ModelPoints.h"

///
using namespace Loki::Test::OnOff;

static bool displayConfig( Cpl::TShell::Context_& context,
                           Cpl::Text::String&     buf,
                           float                  setpoint,
                           uint32_t               statusIntervalMs,
                           float                  onHysteresis,
                           float                  offHysteresis,
                           uint32_t               minOnMs,
                           uint32_t               minOffMs );

static bool displayOutputs( Cpl::TShell::Context_& context, Cpl::Text::String& buf );
static bool displayInputs( Cpl::TShell::Context_& context, Cpl::Text::String& buf );

///////////////////////////
Cmd::Cmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Loki::Test::Api& testManager, Api& m_testHandle ) noexcept
    : Cpl::TShell::Cmd::Command( commandList, verb )
    , m_testManager( testManager )
    , m_testHdl( m_testHandle )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Cmd::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String& outtext    = context.getOutputBuffer();
    bool               io         = true;

    // Display test configuration/status
    if ( tokens.numParameters() == 1 )
    {
        float       setpoint;
        uint32_t    statusIntervalMs;
        float       onHysteresis;
        float       offHysteresis;
        uint32_t    minOnMs;
        uint32_t    minOffMs;
        m_testHdl.getConfig( setpoint, statusIntervalMs, onHysteresis, offHysteresis, minOnMs, minOffMs );
        io &= displayConfig( context, outtext, setpoint, statusIntervalMs, onHysteresis, offHysteresis, minOnMs, minOffMs );
        io &= displayOutputs( context, outtext );
        io &= displayInputs( context, outtext );
        
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Configure
    if ( tokens.numParameters() >= 3 && *( tokens.getParameter( 1 ) ) == 'c' )
    {
        // Get the current config
        float       setpoint;
        uint32_t    statusIntervalMs;
        float       onHysteresis;
        float       offHysteresis;
        uint32_t    minOnMs;
        uint32_t    minOffMs;
        m_testHdl.getConfig( setpoint, statusIntervalMs, onHysteresis, offHysteresis, minOnMs, minOffMs );

        double value;
        if ( !Cpl::Text::a2d( value, tokens.getParameter( 2 ) ) )
        {
            outtext.format( "Bad <setpt> argument: %s", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }
        setpoint = (float) value;

        if ( tokens.numParameters() >= 3 )
        {
            unsigned long value;
            if ( !Cpl::Text::a2ul( value, tokens.getParameter( 3 ) ) )
            {
                outtext.format( "Bad <rptms> argument: %s", tokens.getParameter( 3 ) );
                context.writeFrame( outtext );
                return Command::eERROR_INVALID_ARGS;
            }
            statusIntervalMs = (uint32_t) value;
        }

        if ( tokens.numParameters() >= 4 )
        {
            double value;
            if ( !Cpl::Text::a2d( value, tokens.getParameter( 4 ) ) )
            {
                outtext.format( "Bad <hon> argument: %s", tokens.getParameter( 4 ) );
                context.writeFrame( outtext );
                return Command::eERROR_INVALID_ARGS;
            }
            onHysteresis = (float) value;
        }

        if ( tokens.numParameters() >= 5 )
        {
            double value;
            if ( !Cpl::Text::a2d( value, tokens.getParameter( 5 ) ) )
            {
                outtext.format( "Bad <hoff> argument: %s", tokens.getParameter( 5 ) );
                context.writeFrame( outtext );
                return Command::eERROR_INVALID_ARGS;
            }
            offHysteresis = (float) value;
        }

        if ( tokens.numParameters() >= 6 )
        {
            unsigned long value;
            if ( !Cpl::Text::a2ul( value, tokens.getParameter( 6 ) ) )
            {
                outtext.format( "Bad <mon> argument: %s", tokens.getParameter( 6 ) );
                context.writeFrame( outtext );
                return Command::eERROR_INVALID_ARGS;
            }
            minOnMs = (uint32_t) value;
        }

        if ( tokens.numParameters() >= 7 )
        {
            unsigned long value;
            if ( !Cpl::Text::a2ul( value, tokens.getParameter( 7 ) ) )
            {
                outtext.format( "Bad <moff> argument: %s", tokens.getParameter( 7 ) );
                context.writeFrame( outtext );
                return Command::eERROR_INVALID_ARGS;
            }
            minOffMs = (uint32_t) value;
        }

        // Configure the test
        bool testIsRunning;
        outtext = m_testHdl.getName();
        if ( m_testManager.getCurrentTestStatus( outtext, testIsRunning ) )
        {
            context.writeFrame( "ERROR: Cannot update the Configuration while the test is running." );
            return Command::eERROR_FAILED;
        }
        m_testHdl.configure( setpoint, statusIntervalMs, onHysteresis, offHysteresis, minOnMs, minOffMs );
        io &= displayConfig( context, outtext, setpoint, statusIntervalMs, onHysteresis, offHysteresis, minOnMs, minOffMs );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Start
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "start" ) == 0 )
    {
        if ( !m_testManager.startTest( m_testHdl.getName() ) )
        {
            context.writeFrame( "FAILED to start test" );
            return Command::eERROR_FAILED;
        }
        return Command::eSUCCESS;
    }

    // Stop
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "stop" ) == 0 )
    {
        m_testManager.stopCurrentTest();
        return Command::eSUCCESS;
    }

    // Pause
    if ( tokens.numParameters() == 2 && *( tokens.getParameter( 1 )) == 'p' )
    {
        m_testManager.pauseCurrentTest();
        return Command::eSUCCESS;
    }

    // Resume
    if ( tokens.numParameters() == 2 && *( tokens.getParameter( 1 )) == 'r' )
    {
        m_testManager.resumeCurrentTest();
        return Command::eSUCCESS;
    }

    // Unsupported subcommand
    context.writeFrame( "Unsupported sub-command or bad syntax" );
    return Command::eERROR_INVALID_CMD;
}

//////////////////////////////////////////////
bool displayConfig( Cpl::TShell::Context_& context,
                    Cpl::Text::String&     buf,
                    float                  setpoint,
                    uint32_t               statusIntervalMs,
                    float                  onHysteresis,
                    float                  offHysteresis,
                    uint32_t               minOnMs,
                    uint32_t               minOffMs )
{
    bool io = true;
    buf.format( "Setpoint           = %g 'F", setpoint );
    io &= context.writeFrame( buf );
    buf.format( "Report Interval    = %lu msec", statusIntervalMs );
    io &= context.writeFrame( buf );
    buf.format( "ON Hysteresis      = %g 'F", onHysteresis );
    io &= context.writeFrame( buf );
    buf.format( "OFF Hysteresis     = %g 'F", offHysteresis );
    io &= context.writeFrame( buf );
    buf.format( "Min ON times       = %lu msec", minOnMs );
    io &= context.writeFrame( buf );
    buf.format( "Min OFF times      = %lu msec", minOffMs );

    io &= context.writeFrame( buf );

    return io;
}

bool displayOutputs( Cpl::TShell::Context_& context, Cpl::Text::String& buf )
{
    bool io = true;
    bool val;

    buf = "Water Valve Output = ";
    if ( mp::waterValve.read( val ) )
    {
        buf.formatAppend( "%s", val ? "ON": "off" );
    }
    else
    {
        buf += "<invalid>";
    }
    io &= context.writeFrame( buf );

    buf = "Water Pump Output  = ";
    if ( mp::waterPumpMotor.read( val ) )
    {
        buf.formatAppend( "%s", val ? "ON": "off" );
    }
    else
    {
        buf += "<invalid>";
    }
    io &= context.writeFrame( buf );

    return io;
}

bool displayInputs( Cpl::TShell::Context_& context, Cpl::Text::String& buf )
{
    bool  io = true;
    float val;

    buf = "Wat = ";
    buf = "Water Valve Output = ";
    if ( mp::pvWaterTemp.read( val ) )
    {
        buf.format( "Water Temp.        = %g  'F", val );
    }
    else
    {
        buf.format( "Water Temp.        = <invalid>" );
    }
    io &= context.writeFrame( buf );

    return io;
}
