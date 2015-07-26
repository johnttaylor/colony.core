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
#include "Cpl/Itc/SyncReturnHandler.h"
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
          )
:m_mbox(setLayerMbox)
,m_setHandler(setHandler)
,m_recLayerPtr(0) 
,m_timer(timingSource, *this, &Base::timerExpired) 
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
    Fsm::initialize();

    // Set all of my Point's 'in-use' flags to true (since we will be reading/writing all tuples)
    getMyPoint().setAllInUseState(true);
    }



/////////////////////////////////////////
const Cpl::Container::Key& Base::getKey() const throw()
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
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::request( DefaultMsg& ) [%s]", m_name()) );
    defaultContent();
    msg.returnToSender();
    }


/////////////////////////////////////////
const char* Base::getName(void) const
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::getName() [%s]", m_name()) );
    return m_name;
    }

void Base::setChunkHandle( Rte::Db::Chunk::Handle& src )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::setChunkHandle() [%s]", m_name()) );
    m_chunkHdl = src;
    }

Rte::Db::Chunk::Handle& Base::getChunkHandle(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::getChunkHandle() [%s]", m_name()) );
    return m_chunkHdl;
    }

void Base::notifyWriteDone(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyWriteDone() [%s]", m_name()) );
    sendEvent( evWriteDone );
    }

bool Base::notifyRead( void* srcBuffer, uint32_t dataLen )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. inlen=%ul", m_name(), dataLen) );

    // De-serialize the raw record data one Tuple at a time
    bool             result = true;
    uint8_t*         srcPtr = (uint8_t*)srcBuffer;
    Rte::Point::Api& point  = getMyPoint();
    unsigned         j;
    for(j=0; j<point.getNumTuples(); j++)
        {
        // In theory this is the case of 'appending' new Tuples to an existing Record/Set/Point
        if ( dataLen == 0 )
            {
            m_missingTuples = true;
            m_logger.warning( "Rte::Db::Set::Base::notifyRead[%s] - Minor upgrade to Set (raw record missing %d tuple(s))", m_name(), point.getNumTuples() - j );
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. Minor upgrade to Set (raw record missing %d tuple(s))", m_name(), point.getNumTuples() - j ));
            break;
            }

        // De-serialize the raw record Tuple one Element at a time
        Rte::Tuple::Api& tuple = point.getTuple(j);
        unsigned         i;
        for(i=0; i < tuple.getNumElements(); i++ )
            {
            Rte::Element::Api& element  = tuple.getElement(i);
            uint32_t           elemSize = element.externalSize();
            if ( elemSize > dataLen )
                {
                result = false;
                m_logger.warning( "Rte::Db::Set::Base::notifyRead[%s] - Bad record - missing Element Data", m_name() );
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. Bad record - missing Element Data (element len=%ul, remaining len=%ul)", m_name(), elemSize, dataLen ));
                break;
                }

            element.importElement( srcPtr );
            srcPtr  += elemSize;
            dataLen -= elemSize;                
            }
        }

    // Kick my FSM based on the results of the read
    if ( result )
        {
        sendEvent( evReadDone );
        }
    else
        {
        sendEvent( evDefaultContent );
        }

    return result;
    }


uint32_t Base::fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::fillWriteBuffer() [%s]. maxDataSize=%ul", m_name(), maxDataSize) );

    // Serialize my point's data (one tuple at a time)
    Rte::Point::Api& point     = getMyPoint();
    uint8_t*         dstPtr    = (uint8_t*)dstBuffer;
    uint32_t         filledLen = 0;
    unsigned         j;
    for(j=0; j<point.getNumTuples(); j++)
        {
        // Serialize the Tuple one Element at a time
        Rte::Tuple::Api& tuple = point.getTuple(j);
        unsigned         i;
        for(i=0; i < tuple.getNumElements(); i++ )
            {
            Rte::Element::Api& element  = tuple.getElement(i);
            uint32_t           elemSize = element.externalSize();
            if ( filledLen + elemSize > maxDataSize )
                {
                Cpl::System::FatalError::logf( "Rte::Db::Set::Base::fillWriteBuffer[%s] - Buffer length Error.", m_name() );
                }
            element.exportElement( dstPtr );
            dstPtr    += elemSize;
            filledLen += elemSize;
            }
        }

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::fillWriteBuffer() [%s]. filledLen=%ul", m_name(), filledLen) );
    return filledLen;
    }


/////////////////////////////////////////
void Base::start( Rte::Db::Record::Handler& recordLayer ) throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::start [%s]", m_name()) );

    m_recLayerPtr   = &recordLayer;
    m_missingTuples = false;
    m_loadIsGood    = false;
    sendEvent( evStart );
    }

void Base::stop() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::stop [%s]", m_name()) );
    sendEvent( evStart );
    }

void Base::notifyLoadCompleted(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyLoadCompleted [%s]", m_name()) );

    m_loadIsGood = true;
    sendEvent( evLoadDone );
    }

void Base::notifyLoadFailed(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyLoadFailed [%s]", m_name()) );

    m_loadIsGood = false;
    sendEvent( evDefaultContent );
    }

void Base::defaultContent() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::defaultContent [%s]", m_name()) );
    sendEvent( evStart );
    }

Rte::Db::Record::Api& Base::getRecord() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::getRecord [%s]", m_name()) );
    return *this;
    }


/////////////////////////////////////////
void  Base::pointChanged( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::pointChanged [%s]", m_name()) );
    sendEvent( evDataModified );
    }

void  Base::viewerStopped( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::viewerStopped [%s]", m_name()) );
    sendEvent( evDisconnected );
    }


/////////////////////////////////////////
void Base::connectToModel() throw()  
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::connectToModel [%s]", m_name()) );

    getController().updateModel();
    getViewer().startViewing( true, false ); // args:= use values for change detection, NO initial read (of the model point) 
    }

void Base::defaultSetContents() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::defaultSetContents [%s]", m_name()) );
    defaultSet();
    }

void Base::disconnectFromModel() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::disconnectFromModel [%s]", m_name()) );
    getViewer().stopViewing();
    }

void Base::issueWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::issueWrite [%s]", m_name()) );

    if ( !m_recLayerPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Set::Base::issueWrite - Protocol Error (no record layer reference). Name=%s", m_name() );
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
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::stopTimer [%s]", m_name()) );
    m_timer.stop();
    }

void Base::tellInitialized() throw()
    {
    if ( !m_missingTuples )
        {
        m_setHandler.notifySetInitialized( *this );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellInitialized [%s] -->good load", m_name()) );
        }
    else
        {
        m_setHandler.notifySetConverted( *this );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellInitialized [%s] -->missing tuples, aka auto minor upgrade", m_name()) );
        }
    }

void Base::tellStartCompleted() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStartCompleted [%s]", m_name()) );
    m_setHandler.notifySetStarted( *this );
    }

void Base::tellStarting() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStarting [%s]", m_name()) );
    m_setHandler.notifySetWaiting( *this );
    }

void Base::tellStopped() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStopped [%s]", m_name()) );
    m_setHandler.notifySetStopped( *this );
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
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Event IGNORED:= %s [%s]", getNameByEvent(msg), m_name()) );
            }
        } while( (msg=m_queuedEvent) != FSM_NO_MSG );
    }

void Base::generateInternalEvent( FSM_EVENT_T msg )
    {
    m_queuedEvent = msg;
    }


