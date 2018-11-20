/*-----------------------------------------------------------------------------
* This file is part of the Colony.Base Project.  The Colony.Base Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2018  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Base.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/System/Trace.h"

#define SECT_   "Cpl::Rte::Persistence::Record"


/// Namespace(s)
using namespace Cpl::Rte::Persistence::Record;


/////////////////////////////////////////
Base::Base( Cpl::Container::Map<Api_>&     myRecordList,
            unsigned long                  delayWriteTimeInMsec,
            const char*                    name,
            Cpl::Itc::MailboxServer&       recordLayerMbox,
            Cpl::Log::Api&                 eventLogger
)
    :m_mbox( recordLayerMbox )
    , m_recLayerPtr( 0 )
    , m_timer( recordLayerMbox, *this, &Base::timerExpired )
    , m_writeDelay( delayWriteTimeInMsec )
    , m_name( name )
    , m_logger( eventLogger )
    , m_mismatched( false )
    , m_loadIsGood( false )
    , m_dirty( false )
{
    /// Add myself to the Set list
    myRecordList.insert( *this );

    // Initialize my FSM
    Fsm::initialize();
}



/////////////////////////////////////////
const Cpl::Container::Key& Base::getKey() const throw()
{
    return m_name;
}


/////////////////////////////////////////
const char* Base::getName( void ) const
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::getName() [%s]", m_name()) );
    return m_name;
}

Cpl::Rte::Persistence::Chunk::Handle& Base::getChunkHandle( void )
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
void Base::start( HandlerApi_& recordLayer ) throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::start [%s]", m_name()) );

    m_recLayerPtr   = &recordLayer;
    m_mismatched    = false;
    m_loadIsGood    = false;
    generateEvent( Fsm_evStart );
}

void Base::stop() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::stop [%s]", m_name()) );
    generateEvent( Fsm_evStop );
}

void Base::defaultContent() throw()
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

void Base::setChunkHandle( Cpl::Rte::Persistence::Chunk::Handle& src )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::setChunkHandle() [%s]", m_name()) );
    m_chunkHdl = src;
}



/////////////////////////////////////////
void Base::issueWrite() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::issueWrite [%s]", m_name()) );

    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Base::issueWrite - Protocol Error (no record layer reference). Name=%s", m_name() );
    }

    m_recLayerPtr->write( *this );
}

void Base::markClean() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::markClean [%s]", m_name()) );
    m_dirty = false;
}

void Base::markDirty() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::markDirty [%s]", m_name()) );
    m_dirty = true;
}

void Base::startTimer() throw()
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

void Base::stopTimer() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::stopTimer [%s]", m_name()) );
    m_timer.stop();
}

void Base::tellInitialized() throw()
{
    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Base::tellInitialized - Protocol Error (no record layer reference). Name=%s", m_name() );
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

void Base::tellStartCompleted() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStartCompleted [%s]", m_name()) );
    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Base::tellStartCompleted - Protocol Error (no record layer reference). Name=%s", m_name() );
    }

    m_recLayerPtr->notifyRecordStarted();
}

void Base::tellStarting() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStarting [%s]", m_name()) );
    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Base::tellStarting - Protocol Error (no record layer reference). Name=%s", m_name() );
    }

    m_recLayerPtr->notifyRecordWaiting();
}

void Base::tellStopped() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStopped [%s]", m_name()) );
    if ( !m_recLayerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Base::tellStopped - Protocol Error (no record layer reference). Name=%s", m_name() );
    }

    m_recLayerPtr->notifyRecordStopped();
    m_recLayerPtr = 0;
}


/////////////////////////////////////////
bool Base::isDirty() throw()
{
    return m_dirty;
}

bool Base::isLoadGood() throw()
{
    return m_loadIsGood;
}


///////////////////////////////////////////
void Base::timerExpired( void )
{
    generateEvent( Fsm_evTimerExpired );
}


