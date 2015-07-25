/*-----------------------------------------------------------------------------
* This file is part of the Colony.Base Project.  The Colony.Base Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014, 2015  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Base.h"
#include "Cpl/System/Trace.h"

#define SECT_   "Rte::Db::Set"


/// Namespace(s)
using namespace Rte::Db::Set;


/////////////////////////////////////////
Base::Base( Cpl::Container::Map<ApiLocal>  mySetList,
            HandlerLocal&                  setHandler, 
            unsigned long                  delayWriteTimeInMsec,
            const char*                    name,
            Cpl::Itc::PostApi&             setLayerMbox,
            Cpl::Timer::CounterSource&     timingSource,  
            Cpl::Log::Api&                 eventLogger
          );
:m_mbox(setLayerMbox)
,m_setHandler(setHandler)
,m_recLayerPtr(0) 
,m_timer(timingSource, *this, Base::timerExpired) 
,m_writeDelay(delayWriteTimeInMsec)
,m_name(name)
,m_logger(eventLogger)
,m_missingTuples(false)
,m_loadIsGood(false)
,m_dirty(false)
,m_queuedEvent(FSM_NO_MSG)
    {
    /// Add myself to the Set list
    mySetList.insert( *this );

    // Initialize my FSM
    initialize();
    }



/////////////////////////////////////////
const Key& Base::getKey() const throw()
    {
    return m_name;
    }


/////////////////////////////////////////
void Base::defaultSetContent() throw()   
    {
    DefaultPayload                payload;
    Cpl::Itc::SyncReturnHandler   srh;
    DefaultMsg                    msg(*this,payload,srh);
    m_mbox.postSync(msg);
    }

void Base::request( DefaultMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::request( DefaultMsg& )") );
    defaultContent();
    msg.returnToSender();
    }


/////////////////////////////////////////
const char* Base::getName(void) const
void Base::setChunkHandle( Rte::Db::Chunk::Handle& src )
Rte::Db::Chunk::Handle& Base::getChunkHandle(void)
void Base::notifyRead( void* srcBuffer, uint32_t dataLen )
uint32_t Base::fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize )
void Base::notifyWriteDone(void)

/////////////////////////////////////////
void Base::start( Rte::Db::Record::Handler& recordLayer ) throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::start") );

    m_recLayerPtr   = recordLayer;
    m_missingTuples = false;
    m_loadIsGood    = false;
    sendEvent( evStart );
    }

void Base::stop() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::stop") );
    sendEvent( evStart );
    }

void Base::notifyLoadCompleted(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyLoadCompleted") );

    m_loadIsGood = true;
    sendEvent( evLoadDone );
    }

void Base::notifyLoadFailed(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyLoadFailed") );

    m_loadIsGood = false;
    sendEvent( evDefaultContent );
    }

void Base::defaultContent() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::defaultContent") );
    sendEvent( evStart );
    }

Rte::Db::Record::Api& Base::getRecord() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::getRecord") );
    return *this;
    }


/////////////////////////////////////////
void  Base::pointChanged( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::pointChanged") );
    sendEvent( evDataModified );
    }

void  Base::viewerStopped( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::viewerStopped") );
    sendEvent( evDisconnected );
    }


/////////////////////////////////////////
void Base::connectToModel() throw()  
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::connectToModel") );

    getController().updateModel();
    getViewer().startViewing( true, false ); // args:= use values for change detection, NO initial read (of the model point) 
    }

void Base::defaultSetContents() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::defaultSetContents") );
    defaultSet();
    }

void Base::disconnectFromModel() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::disconnectFromModel") );
    getViewer().stopViewing();
    }

void Base::issueWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::issueWrite") );

    if ( !m_recLayerPtr )
        {
        Cpl::System::FatalError::log( "Rte::Db::Set::Base::issueWrite - Protocol Error (no record layer reference)." );
        }

    m_recLayerPtr->write( *this );
    }

void Base::markClean() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::markClean") );
    m_dirty = false; 
    }

void Base::markDirty() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::markDirty") );
    m_dirty = true; 
    }

void Base::startTimer() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::startTimer") );

    // Generate an internal event if there is NO write delay
    if ( m_writeDelay == 0 )
        {
        generateInternalEvent( evTimerExpired );
        }

    // Start delay timer
    else
        {
        m_timer.start( m_writeDelay );
        }
    }

void Base::stopTimer() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::stopTimer") );
    m_timer.stop();
    }

void Base::tellInitialized() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellInitialized") );
    m_setHandler.notifySetInitialized( *this );
    }

void Base::tellStartCompleted() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStartCompleted") );
    m_setHandler.notifySetStarted( *this );
    }

void Base::tellStarting() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStarting") );
    m_setHandler.notifyStarting( *this );
    }

void Base::tellStopped() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStopped") );
    m_setHandler.notifyStopped( *this );
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
    sendEvent( evTimerExpired );
    }


void Base::sendEvent( FSM_EVENT_T msg )
    {
    // I have an 'event queue' so that if an action generates an event the event 
    // can be queued until current event finishes processing, i.e. enforce run-to-
    // completion semantics on event processing. The event queue really isn't a 
    // queue - just a single variable - since by design of the FSM at most only 
    // one event can be 'self generated' on any given event processing.
    do 
        {
        m_queuedEvent = FSM_NO_MSG;
        if ( processEvent(msg) == 0 )
            {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Event IGNORED:= %s", getNameByEvent(msg)) );
            }
        } while( (msg=m_queuedEvent) != FSM_NO_MSG );
    }

void Base::generateInternalEvent( FSM_EVENT_T msg )
    {
    m_queuedEvent = msg;
    }


