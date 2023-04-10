/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Manager.h"
#include "TestApi.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/Text/FString.h"

using namespace Loki::Test;


////////////////////////////////////////////////////////////////////////////////
Manager::Manager( Cpl::Dm::MailboxServer&                   myMbox,
                  Cpl::Container::Map<Loki::Test::TestApi>& listOfTests )
    : Cpl::Itc::CloseSync( *( (Cpl::Itc::PostApi*) &myMbox ) )
    , m_tests( listOfTests )
    , m_currentTestPtr( 0 )
    , m_opened( false )
{
}


void Manager::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
{
    if ( m_opened )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "Loki::Test::Manager: open() called when already opened" ) );
    }
    else
    {
        // Housekeeping
        m_opened  = true;

        // initialize my tests
        TestApi* itemPtr = m_tests.first();
        while ( itemPtr )
        {
            itemPtr->intialize();
            itemPtr = m_tests.next( *itemPtr );
        }
    }

    // Return the message
    msg.returnToSender();
}

void Manager::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
{
    if ( !m_opened )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "Loki::Test::Manager: close() called when not opened" ) );
    }
    else
    {
        // Housekeeping
        m_opened  = false;

        // shutdown my tests
        TestApi* itemPtr = m_tests.first();
        while ( itemPtr )
        {
            itemPtr->shutdown();
            itemPtr = m_tests.next( *itemPtr );
        }
    }

    // Return the message
    msg.returnToSender();
}

//////////////////////////////////////////////////////////////////////////////
void Manager::request( StartTestMsg& msg )
{
    StartTestRequest::Payload& payload = msg.getPayload();
    payload.m_success = false;

    // Look-up the test by name
    Cpl::Container::KeyLiteralString myKey( payload.m_testName );
    TestApi* test = m_tests.find( myKey );
    if ( test != 0 )
    {
        payload.m_success = true;

        // Stop the current test if there is one
        if ( m_currentTestPtr )
        {
            m_currentTestPtr->stop();
        }

        // Start the new test
        m_currentTestPtr = test;
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "Starting: %s", m_currentTestPtr->getName() ) );
        m_currentTestPtr->start();
    }

    msg.returnToSender();
}

void Manager::request( StopTestMsg & msg )
{
    if ( m_currentTestPtr )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "Stopping: %s", m_currentTestPtr->getName() ) );
        m_currentTestPtr->stop();
        m_currentTestPtr = 0;
    }
    msg.returnToSender();
}

void Manager::request( PauseTestMsg & msg )
{
    if ( m_currentTestPtr && !m_currentTestPtr->isPaused() )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "Pausing: %s", m_currentTestPtr->getName() ) );
        m_currentTestPtr->pause();
    }
    msg.returnToSender();
}

void Manager::request( ResumeTestMsg & msg )
{
    if ( m_currentTestPtr && m_currentTestPtr->isPaused() )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ( "Resuming: %s", m_currentTestPtr->getName() ) );
        m_currentTestPtr->resume();
    }
    msg.returnToSender();
}

void Manager::request( GetTestStatusMsg & msg )
{
    GetTestStatusRequest::Payload& payload = msg.getPayload();
    payload.m_testExecuting = false;
    if ( m_currentTestPtr )
    {
        payload.m_testExecuting = true;
        payload.m_testName      = m_currentTestPtr->getName();
        payload.m_testExecuting = !m_currentTestPtr->isPaused();
    }
    msg.returnToSender();
}

const Cpl::Container::Map<Loki::Test::TestApi>& Manager::getAvailableTests() const noexcept
{
    // NO ITC/Critical-Section required since this is all static/const data
    return m_tests;
}

//////////////////////////////////////////////////////////////////////////////
bool Manager::startTest( const char* testName ) noexcept
{
    StartTestRequest::Payload      payload( testName );
    Cpl::Itc::SyncReturnHandler    srh;
    StartTestMsg                   msg( *this, payload, srh );
    m_mbox.postSync( msg );

    return payload.m_success;
}


void Manager::stopCurrentTest() noexcept
{
    StopTestRequest::Payload     payload;
    Cpl::Itc::SyncReturnHandler  srh;
    StopTestMsg                  msg( *this, payload, srh );
    m_mbox.postSync( msg );
}

void Manager::pauseCurrentTest() noexcept
{
    PauseTestRequest::Payload     payload;
    Cpl::Itc::SyncReturnHandler   srh;
    PauseTestMsg                  msg( *this, payload, srh );
    m_mbox.postSync( msg );
}

void Manager::resumeCurrentTest() noexcept
{
    ResumeTestRequest::Payload  payload;
    Cpl::Itc::SyncReturnHandler srh;
    ResumeTestMsg               msg( *this, payload, srh );
    m_mbox.postSync( msg );
}

bool Manager::getCurrentTestStatus( Cpl::Text::String& testName, bool& testIsRunning ) noexcept
{
    GetTestStatusRequest::Payload  payload;
    Cpl::Itc::SyncReturnHandler    srh;
    GetTestStatusMsg               msg( *this, payload, srh );
    m_mbox.postSync( msg );

    testName      = payload.m_testName;
    testIsRunning = payload.m_running;
    return payload.m_testExecuting;
}


