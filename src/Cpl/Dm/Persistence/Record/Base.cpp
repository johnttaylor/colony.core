/*-----------------------------------------------------------------------------
* This file is part of the Colony.Base Project.  The Colony.Base Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Base.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/Persistence/Framing.h"

#define SECT_   "Cpl::Dm::Persistence::Record"


/// Namespace(s)
using namespace Cpl::Dm::Persistence::Record;


/////////////////////////////////////////
Base::Base( Cpl::Container::Map<Api_>&     myRecordList,
            unsigned long                  delayWriteTimeInMsec,
            const char*                    name,
            Cpl::Dm::MailboxServer&        recordLayerMbox
)
    :m_mbox( recordLayerMbox )
    , m_recLayerPtr( 0 )
    , m_timer( recordLayerMbox, *this, &Base::timerExpired )
    , m_writeDelay( delayWriteTimeInMsec )
    , m_name( name )
    , m_mismatched( false )
    , m_loadIsGood( false )
    , m_dirty( false )
{
    /// Add myself to the Record list
    myRecordList.insert( *this );

    // Initialize my FSM
    Fsm::initialize();
}



/////////////////////////////////////////
void Base::registerModelPoint( ModelPointReference_t& mpToAdd )
{
    m_points.put( mpToAdd );
}

const Cpl::Container::Key& Base::getKey() const noexcept
{
    return m_name;
}


/////////////////////////////////////////
const char* Base::getName( void ) const
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::getName() [%s]", m_name()) );
    return m_name;
}

Cpl::Dm::Persistence::Chunk::Handle& Base::getChunkHandle( void )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::getChunkHandle() [%s]", m_name()) );
    return m_chunkHdl;
}

void Base::notifyWriteDone( void )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyWriteDone() [%s]", m_name()) );
    generateEvent( Fsm_evWriteDone );
}

/////////////////////////////////////////
void Base::start( HandlerApi_& recordLayer ) noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::start [%s]", m_name()) );

    m_recLayerPtr   = &recordLayer;
    m_mismatched    = false;
    m_loadIsGood    = false;
    resetHistoryStarting();         // Ensure the History states/status gets reset when re-starting
    resetHistoryActive();
    generateEvent( Fsm_evStart );
}

void Base::stop() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::stop [%s]", m_name()) );
    generateEvent( Fsm_evStop );
}

void Base::defaultContent() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::defaultContent [%s]", m_name()) );
    generateEvent( Fsm_evDefault );
}

void Base::notifyLoadCompleted( void )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyLoadCompleted [%s]", m_name()) );

    m_loadIsGood = true;
    generateEvent( Fsm_evLoadDone );
}

void Base::notifyLoadFailed( void )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyLoadFailed [%s]", m_name()) );

    m_loadIsGood = false;
    generateEvent( Fsm_evLoadDone );
}

void Base::setChunkHandle( Cpl::Dm::Persistence::Chunk::Handle& src )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::setChunkHandle() [%s]", m_name()) );
    m_chunkHdl = src;
}

uint32_t Base::fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::fillWriteBuffer() [%s]. maxDataSize=%lu", m_name(), maxDataSize) );

    // Serialize my Record's data (one MP at a time)
    ModelPointReference_t* mpRefPtr = m_points.head();
    uint8_t*               dstPtr    = (uint8_t*) dstBuffer;
    uint32_t               filledLen = 0;
    while ( mpRefPtr )
    {
        Cpl::Dm::ModelPoint& mp = mpRefPtr->m_reference;

        // Write the MP's name to the record
        const char* mpName    = mp.getName();
        uint16_t    mpNameLen = (uint16_t) strlen( mpName );
        if ( filledLen + Cpl::Dm::Persistence::eNLEN_SIZE + mpNameLen > maxDataSize )
        {
            Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Base::fillWriteBuffer[%s] - Buffer length Error when writing MP(%s) name.", m_name(), mpName );
        }
        memcpy( dstPtr, &mpNameLen, Cpl::Dm::Persistence::eNLEN_SIZE );
        memcpy( dstPtr + Cpl::Dm::Persistence::eNLEN_SIZE, mpName, mpNameLen );
        dstPtr    += Cpl::Dm::Persistence::eNLEN_SIZE + mpNameLen;
        filledLen += Cpl::Dm::Persistence::eNLEN_SIZE + mpNameLen;

        // Write the MP's data/state to the record
        size_t bytesWritten = mp.exportData( dstPtr, maxDataSize - filledLen );
        if ( bytesWritten == 0 )
        {
            Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Base::fillWriteBuffer[%s] - Buffer length Error when writing MP(%s) data.", m_name(), mpName );
        }
        dstPtr    += bytesWritten;
        filledLen += bytesWritten;

        // Get the next MP to write
        mpRefPtr = m_points.next( *mpRefPtr );
    }

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::fillWriteBuffer() [%s]. filledLen=%lu", m_name(), filledLen) );
    return filledLen;
}

bool Base::notifyRead( void* srcBuffer, uint32_t dataLen )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. inlen=%lu", m_name(), dataLen) );

    // De-serialize the raw record data one MP at a time
    bool                   result   = true;
    uint8_t*               srcPtr   = (uint8_t*) srcBuffer;
    ModelPointReference_t* mpRefPtr = m_points.head();
    while ( mpRefPtr )
    {
        Cpl::Dm::ModelPoint& mp = mpRefPtr->m_reference;
        
        // Get the expected Name
        const char* expectedMpName    = mp.getName();
        uint16_t    expectedMpNameLen = (uint16_t)strlen( expectedMpName );

        // In theory this is the case of 'appending' new Model Point(s) to an existing Record
        // NOTE: In this scenario we do NOT 'fail' the read so that all previously read MP(s) retain their values from the raw record data
        if ( dataLen == 0 )
        {
            m_mismatched = true;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. Minor upgrade to Record (raw record missing %d mp(%s))", m_name(), expectedMpName) );
            break;
        }

        // Get actual MP name
        if ( dataLen < ((uint32_t)(expectedMpNameLen + Cpl::Dm::Persistence::eNLEN_SIZE)) )
        {
            m_mismatched = true;
            result       = false;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. Bad raw record length, unable to read MP name field (expected name=%s)", m_name(), expectedMpName) );
            break;
        }

        // Is this the expected MP?
        if ( memcmp( srcPtr + Cpl::Dm::Persistence::eNLEN_SIZE, expectedMpName, expectedMpNameLen ) != 0 )
        {
            m_mismatched = true;
            result       = false;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. Unexpected MP name (%.*s) (expected name=%s)", m_name(), srcPtr + Cpl::Dm::Persistence::eNLEN_SIZE, expectedMpName) );
            break;
        }

        // Read the MP's data
        srcPtr          += Cpl::Dm::Persistence::eNLEN_SIZE + expectedMpNameLen;
        dataLen         -= Cpl::Dm::Persistence::eNLEN_SIZE + expectedMpNameLen;
        size_t bytesRead = mp.importData( srcPtr, dataLen );
        if ( bytesRead == 0 )
        {
            m_mismatched = true;
            result       = false;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. Bad Record - missing MP (expected name=%s)", m_name(), expectedMpName) );
            break;
        }

        // Get next MP to read
        srcPtr  += bytesRead;
        dataLen -= bytesRead;
        mpRefPtr = m_points.next( *mpRefPtr );
    }

    // Default the record if there was a read failure, i.e. guarantee the state of the Record if a read error occurred
    if ( result == false )
    {
        defaultData();
    }
    
    // Kick my FSM now that I am done with read and return my read-success result to the Record Handler
    generateEvent( Fsm_evReadDone );
    return result;
}

/////////////////////////////////////////
void Base::issueWrite() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::issueWrite [%s]", m_name()) );

    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Base::issueWrite - Protocol Error (no record layer reference). Name=%s", m_name() );
    }

    m_recLayerPtr->write( *this );
}

void Base::markClean() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::markClean [%s]", m_name()) );
    m_dirty = false;
}

void Base::markDirty() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::markDirty [%s]", m_name()) );
    m_dirty = true;
}

void Base::startTimer() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::startTimer [%s]", m_name()) );

    // Generate an internal event if there is NO write delay
    if ( m_writeDelay == 0 )
    {
        generateEvent( Fsm_evTimerExpired );
    }

    // Start delay timer
    else
    {
        m_timer.start( m_writeDelay );
    }
}

void Base::stopTimer() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::stopTimer [%s]", m_name()) );
    m_timer.stop();
}

void Base::tellInitialized() noexcept
{
    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Base::tellInitialized - Protocol Error (no record layer reference). Name=%s", m_name() );
    }

    if ( !m_mismatched )
    {
        m_recLayerPtr->notifyRecordInitialized();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellInitialized [%s] -->good load", m_name()) );
    }
    else
    {
        m_recLayerPtr->notifyRecordConverted();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellInitialized [%s] -->media content did not match expected content", m_name()) );
    }
}

void Base::tellStartCompleted() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStartCompleted [%s]", m_name()) );
    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Base::tellStartCompleted - Protocol Error (no record layer reference). Name=%s", m_name() );
    }

    m_recLayerPtr->notifyRecordStarted();
}

void Base::tellStarting() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStarting [%s]", m_name()) );
    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Base::tellStarting - Protocol Error (no record layer reference). Name=%s", m_name() );
    }

    m_recLayerPtr->notifyRecordWaiting();
}

void Base::tellStopped() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStopped [%s]", m_name()) );
    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Base::tellStopped - Protocol Error (no record layer reference). Name=%s", m_name() );
    }

    m_recLayerPtr->notifyRecordStopped();
    m_recLayerPtr = 0;
}


/////////////////////////////////////////
bool Base::isDirty() noexcept
{
    return m_dirty;
}

bool Base::isLoadGood() noexcept
{
    return m_loadIsGood;
}


///////////////////////////////////////////
void Base::timerExpired( void )
{
    generateEvent( Fsm_evTimerExpired );
}

