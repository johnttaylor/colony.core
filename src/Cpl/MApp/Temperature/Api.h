#ifndef Cpl_MApp_Temperature_Api_h_
#define Cpl_MApp_Temperature_Api_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "colony_config.h"
#include "Cpl/MApp/Api.h"
#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Timer.h"



///
namespace Cpl {
///
namespace MApp {
///
namespace Temperature {


/** This concrete class implements MApp the polls a model point for a 
    temperature value and periodically writes the value to the trace output.
    The MApp also collects some metrics and has the option to display temperature
    in degrees Fahrenheit or Celsius. 

    NOTE: Multiple of instance of the class can be created - as long as each
          instance has a unique 'name'.

    The class is intended to be example/template for an MApp - not that it does
    anything particular noteworthy,
 */
class Api : public Cpl::MApp::Api, public Cpl::System::Timer
{
public:
    /// The default MApp Name
    static constexpr const char* DEFAULT_NAME = "temperature";

    /** Usage string (recommended that lines do not exceed 80 chars)
                                                         1         2         3         4         5         6         7         8
                                                12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* USAGE       = "args: [<samplems> [<displayms> [F|C]]]\n"
                                               "  <samplems>   milliseconds between samples. Default is 100ms\n"
                                               "  <displayms>  milliseconds between outputs. Default is 5,000ms\n"
                                               "  F|C          'F' use Fahrenheit, 'C' use Celsius. Default is 'F'";

    /** Description string (recommended that lines do not exceed 80 chars)
                                                         1         2         3         4         5         6         7         8
                                                12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* DESCRIPTION = "Periodically Samples temperature and displays sample/metric values.";


public:
    /// Constructor. Note: The myMbox argument is only needed because the class uses a SW timer
    Api( Cpl::Container::SList<Api>&    mappList,
         Cpl::Dm::MailboxServer&        myMbox,
         Cpl::Dm::Mp::Float&            srcTemperatureMp,
         const char*                    name = DEFAULT_NAME );

protected:
    /// See Cpl::MApp::Api
    void intialize_() noexcept;

    /// See Cpl::MApp::Api
    void start_() noexcept;

    /// See Cpl::MApp::Api
    void stop_() noexcept;

    /// See Cpl::MApp::Api
    void shutdown_() noexcept;

protected:
    /// Timer expired callback
    void expired( void ) noexcept;


protected:
    /// Temperature Input.  Assumed units is Celsius
    Cpl::Dm::Mp::Float& m_temperature;

    /// Maximum Temp. sampled
    float               m_maxTemp;

    /// Minimum Temp. sampled
    float               m_minTemp;

    /// Average Temp. sampled
    float               m_avgTemp;

    /// Number of samples
    float               m_avgTemp;

    /// Sample time in msecs
    uint32_t            m_sampleMs;

    /// Display time in msecs
    uint32_t            m_displayMs;

    /// Time marker of last display action
    uint32_t            m_timeMarkerMs;

    /// When true the values are displayed in Fahrenheit
    bool                m_fahrenheit;
};

};      // end namespaces
};
};
#endif  // end header latch
