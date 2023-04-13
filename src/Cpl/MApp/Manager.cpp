/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Manager.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/Text/FString.h"

using namespace Cpl::MApp;

static Api* searchList( Cpl::Container::SList<Api>& listToSearch,
                        const char*                 nameToFind );

////////////////////////////////////////////////////////////////////////////////
Manager::Manager( Cpl::Dm::MailboxServer&     myMbox,
                  Cpl::Container::SList<Api>& listOfMApps )
    : Cpl::Itc::CloseSync( *((Cpl::Itc::PostApi*) &myMbox) )
    , m_inactiveMApps( listOfMApps )
    , m_startedMApps()
    , m_opened( false )
{
}


void Manager::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
{
    if ( m_opened )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ("Cpl::MApp::Manager: open() called when already opened") );
    }
    else
    {
        // Housekeeping
        m_opened  = true;

        // initialize my instances
        Api* itemPtr = m_inactiveMApps.first();
        while ( itemPtr )
        {
            itemPtr->intialize_();
            itemPtr = m_inactiveMApps.next( *itemPtr );
        }
    }

    // Return the message
    msg.returnToSender();
}

void Manager::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
{
    if ( !m_opened )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ("Cpl::MApp::Manager: close() called when not opened") );
    }
    else
    {
        // Housekeeping
        m_opened  = false;

        // shutdown my inactive instances
        Api* itemPtr = m_inactiveMApps.first();
        while ( itemPtr )
        {
            itemPtr->shutdown_();
            itemPtr = m_inactiveMApps.next( *itemPtr );
        }

        // shutdown my started instances (and return the instance to the inactive list (for the use case of restarting the Manager)
        itemPtr = m_startedMApps.get();
        while ( itemPtr )
        {
            itemPtr->stop_();
            itemPtr->shutdown_();

            // Return the instance to the inactive list
            m_inactiveMApps.push( *itemPtr );

            // Get the next item
            itemPtr = m_startedMApps.get();
        }
    }

    // Return the message
    msg.returnToSender();
}
//////////////////////////////////////////////////////////////////////////////
void Manager::request( StartMAppMsg& msg )
{
    StartMAppRequest::Payload& payload = msg.getPayload();
    payload.success = false;

    // Look-up the test by name
    Api* mapp = searchList( m_inactiveMApps, payload.mappName );
    if ( mapp != nullptr )
    {
        payload.success = true;

        // Put the MApp instance into the started list
        m_inactiveMApps.remove( *mapp );
        m_startedMApps.put( *mapp );

        // Start the new test
        const char* args = payload.mappArgs;
        if ( args == nullptr )
        {
            args = "";
        }
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ("Starting: %s %s", mapp->getName(), args) );
        mapp->start_( args );
    }

    msg.returnToSender();
}

void Manager::request( StopMAppMsg & msg )
{
    StopMAppRequest::Payload& payload = msg.getPayload();

    // Look-up the test by name in the 'running' list
    Api* mapp = searchList( m_startedMApps, payload.mappName );
    if ( mapp != nullptr )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_LOKI_TEST_TRACE_SECTION, ("Stopping: %s", mapp->getName()) );
        mapp->stop_();

        // Return the instance to the inactive list
        m_startedMApps.remove( *mapp );
        m_inactiveMApps.push( *mapp );
    }

    msg.returnToSender();
}

void Manager::request( GetAvailableMAppMsg& msg )
{
    GetAvailableMAppRequest::Payload& payload = msg.getPayload();
    size_t idx                                = 0;
    size_t maxElems                           = payload.dstMaxElements;

    // Walk the inactive list
    Api* item = m_inactiveMApps.first();
    while ( item && maxElems )
    {
        payload.dstList[idx] = item;
        idx++;
        maxElems--;
        payload.numElements++;

        item = m_inactiveMApps.next( *item );
    }

    // Walk the running list
    item = m_startedMApps.first();
    while ( item && maxElems )
    {
        payload.dstList[idx] = item;
        idx++;
        maxElems--;
        payload.numElements++;

        item = m_startedMApps.next( *item );
    }

    msg.returnToSender();
}

void Manager::request( GetStartedMAppMsg & msg )
{
    GetStartedMAppRequest::Payload& payload = msg.getPayload();
    size_t idx                              = 0;
    size_t maxElems                         = payload.dstMaxElements;

    // Walk the running list
    Api* item = m_startedMApps.first();
    while ( item && maxElems )
    {
        payload.dstList[idx] = item;
        idx++;
        maxElems--;
        payload.numElements++;

        item = m_startedMApps.next( *item );
    }

    msg.returnToSender();
}

void Manager::request( LookupMAppMsg & msg )
{
    LookupMAppRequest::Payload& payload = msg.getPayload();

    // Search both lists
    Api* mapp = searchList( m_inactiveMApps, payload.name );
    if ( mapp == nullptr )
    {
        mapp = searchList( m_startedMApps, payload.name );
    }
    payload.foundInstance = mapp;

    msg.returnToSender();
}

//////////////////////////////////////////////////////////////////////////////
bool Manager::startMApp( const char* name, const char* args ) noexcept
{
    StartMAppRequest::Payload      payload( name, args );
    Cpl::Itc::SyncReturnHandler    srh;
    StartMAppMsg                   msg( *this, payload, srh );
    m_mbox.postSync( msg );

    return payload.success;
}


void Manager::stopMApp( const char* name ) noexcept
{
    StopMAppRequest::Payload     payload( name );
    Cpl::Itc::SyncReturnHandler  srh;
    StopMAppMsg                  msg( *this, payload, srh );
    m_mbox.postSync( msg );
}

bool Manager::getAvailableMApps( Cpl::MApp::Api* dstList[], size_t dstMaxElements, size_t& numElemsFound ) noexcept
{
    GetAvailableMAppRequest::Payload  payload(dstList,dstMaxElements);
    Cpl::Itc::SyncReturnHandler       srh;
    GetAvailableMAppMsg               msg( *this, payload, srh );
    m_mbox.postSync( msg );

    numElemsFound = payload.numElements;
    return payload.dstMaxElements != 0;
}

bool Manager::getStartedMApps( Cpl::MApp::Api* dstList[], size_t dstMaxElements, size_t& numElemsFound ) noexcept
{
    GetStartedMAppRequest::Payload  payload( dstList, dstMaxElements );
    Cpl::Itc::SyncReturnHandler     srh;
    GetStartedMAppMsg               msg( *this, payload, srh );
    m_mbox.postSync( msg );

    numElemsFound = payload.numElements;
    return payload.dstMaxElements != 0;
}

///////////////////////////////
Api* searchList( Cpl::Container::SList<Api>&listToSearch,
                 const char*                 nameToFind )
{
    Api* itemPtr = listToSearch.first();
    while ( itemPtr )
    {
        if ( strcmp( itemPtr->getName(), nameToFind ) == 0 )
        {
            return itemPtr;
        }
        itemPtr = listToSearch.next( *itemPtr );
    }

    return nullptr;
}
