#ifndef Loki_Test_Manager_h_
#define Loki_Test_Manager_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Loki/Test/Api.h"
#include "Loki/Test/Requests.h"

///
namespace Loki {
///
namespace Test {


/** This concrete class implements the Test Manager.  The Test Manager is 
    the 'public facing' entity that is responsible for starting, stopping, etc.
    tests.  It is also responsible for ensure that only AT MOST ONE test is 
    executing at any given time.
 */
class Manager: public Api, public Cpl::Itc::CloseSync,
    public StartTestRequest,
    public StopTestRequest,
    public PauseTestRequest,
    public ResumeTestRequest,
    public GetTestStatusRequest
{
public:
    /// Constructor
    Manager( Cpl::Dm::MailboxServer&                   myMbox, 
             Cpl::Container::Map<Loki::Test::TestApi>& listOfTests );


public:
    /// Perform the in-thread initialization of the service
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /// Performs the in-thread shutdown of the service
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


public:
    /// See Loki::Test::Api
    bool startTest( const char* testName ) noexcept;

    /// See Loki::Test::Api
    void stopCurrentTest() noexcept;

    /// See Loki::Test::Api
    void pauseCurrentTest() noexcept;

    /// See Loki::Test::Api
    void resumeCurrentTest() noexcept;

    /// See Loki::Test::Api
    bool getCurrentTestStatus( Cpl::Text::String& testName, bool& testIsRunning ) noexcept;

    /// See Loki::Test::Api
    const Cpl::Container::Map<Loki::Test::TestApi>& getAvailableTests() const noexcept;

public:
    /// Set Loki::Test::Requests
    void request( StartTestMsg& msg );

    /// Set Loki::Test::Requests
    void request( StopTestMsg& msg );

    /// Set Loki::Test::Requests
    void request( PauseTestMsg& msg );

    /// Set Loki::Test::Requests
    void request( ResumeTestMsg& msg );

    /// Set Loki::Test::Requests
    void request( GetTestStatusMsg& msg );

protected:
    /// List of tests
    Cpl::Container::Map<TestApi>& m_tests;

    /// Current test that is executing
    TestApi*                    m_currentTestPtr;

    /// My open/close state
    bool                        m_opened;

};

};      // end namespaces
};
#endif  // end header latch