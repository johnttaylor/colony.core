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

#define SECT_   "Rte::Db::Record"


/// Namespace(s)
using namespace Rte::Db::Record;


/////////////////////////////////////////
Base::Base( Rte::Point::Api&               myNullPoint4Viewer, 
            Rte::Point::Model::Api&        modelPoint,
            Cpl::Container::Map<ApiLocal>& myRecordList,
            unsigned long                  delayWriteTimeInMsec,
            const char*                    name,
            Cpl::Itc::PostApi&             recordLayerMbox,
            Cpl::Timer::CounterSource&     timingSource,  
            Cpl::Log::Api&                 eventLogger
          )
:Rte::Point::Viewer::Base( myNullPoint4Viewer, modelPoint, recordLayerMbox )
,m_mbox(recordLayerMbox)
,m_recLayerPtr(0) 
,m_timer(timingSource, *this, &Base::timerExpired) 
,m_writeDelay(delayWriteTimeInMsec)
,m_name(name)
,m_logger(eventLogger)
,m_missingTuples(false)
,m_loadIsGood(false)
,m_dirty(false)
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
const char* Base::getName(void) const
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::getName() [%s]", m_name()) );
    return m_name;
    }

Rte::Db::Chunk::Handle& Base::getChunkHandle(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::getChunkHandle() [%s]", m_name()) );
    return m_chunkHdl;
    }

void Base::notifyWriteDone(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyWriteDone() [%s]", m_name()) );
    generateEvent( Fsm_evWriteDone );
    }

uint32_t Base::fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::fillWriteBuffer() [%s]. maxDataSize=%lu", m_name(), maxDataSize) );

    // Serialize my point's data (one tuple at a time)
    Rte::Point::Api& point     = getModelPoint().getMyPoint_nonThreadSafe();
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
                Cpl::System::FatalError::logf( "Rte::Db::Record::Base::fillWriteBuffer[%s] - Buffer length Error.", m_name() );
                }
            element.exportElement( dstPtr );
            dstPtr    += elemSize;
            filledLen += elemSize;
            }
        }

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::fillWriteBuffer() [%s]. filledLen=%lu", m_name(), filledLen) );
    return filledLen;
    }


/////////////////////////////////////////
void Base::start( HandlerLocal& recordLayer ) throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::start [%s]", m_name()) );

    m_recLayerPtr   = &recordLayer;
    m_missingTuples = false;
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

void Base::notifyLoadCompleted(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyLoadCompleted [%s]", m_name()) );

    m_loadIsGood = true;
    generateEvent( Fsm_evLoadDone );
    }

void Base::notifyLoadFailed(void)
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyLoadFailed [%s]", m_name()) );

    m_loadIsGood = false;
    generateEvent( Fsm_evLoadDone );
    }

void Base::setChunkHandle( Rte::Db::Chunk::Handle& src )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::setChunkHandle() [%s]", m_name()) );
    m_chunkHdl = src;
    }

bool Base::notifyRead( void* srcBuffer, uint32_t dataLen )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. inlen=%lu", m_name(), dataLen) );

    // De-serialize the raw record data one Tuple at a time
    bool             result = true;
    uint8_t*         srcPtr = (uint8_t*)srcBuffer;
    Rte::Point::Api& point  = getModelPoint().getMyPoint_nonThreadSafe();
    unsigned         j;
    for(j=0; j<point.getNumTuples(); j++)
        {
        // In theory this is the case of 'appending' new Tuples to an existing Record/Set/Point
        if ( dataLen == 0 )
            {
            m_missingTuples = true;
            m_logger.warning( "Rte::Db::Record::Base::notifyRead[%s] - Minor upgrade to Set (raw record missing %d tuple(s))", m_name(), point.getNumTuples() - j );
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
                m_logger.warning( "Rte::Db::Record::Base::notifyRead[%s] - Bad record - missing Element Data", m_name() );
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::notifyRead() [%s]. Bad record - missing Element Data (element len=%lu, remaining len=%lu)", m_name(), elemSize, dataLen ));
                break;
                }

            element.importElement( srcPtr );
            srcPtr  += elemSize;
            dataLen -= elemSize;                
            }
        }

    // Mark the model point as have being updated
    getModelPoint().touch_nonThreadSafe();
    getMyPoint().copyAllSequenceNumbersFrom( point );

    // Kick my FSM based on the results of the read
    if ( result )
        {
        generateEvent( Fsm_evReadDone );
        }
    else
        {
        generateEvent( Fsm_evDefault );
        }

    return result;
    }


/////////////////////////////////////////
void Base::modelHasChanged( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::modelHasChanged [%s]", m_name()) );
    generateEvent( Fsm_evDataModified );
    }

void Base::viewerHasStopped( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::viewerHasStopped [%s]", m_name()) );
    generateEvent( Fsm_evDisconnected );
    }


/////////////////////////////////////////
void Base::connectToModel() throw()  
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::connectToModel [%s]", m_name()) );

    startViewing( false, false, true ); // args:= use sequence numbers for change detection, NO initial read (of the model point), SKIP initialization the sequence numbers 
    }

void Base::defaultData() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::defaultData [%s]", m_name()) );
    getModelPoint().defaultContents_nonThreadSafe();
    getMyPoint().copyAllSequenceNumbersFrom( getModelPoint().getMyPoint_nonThreadSafe() );
    }

void Base::disconnectFromModel() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::disconnectFromModel [%s]", m_name()) );
    stopViewing();
    }

void Base::issueWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::issueWrite [%s]", m_name()) );

    if ( !m_recLayerPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Base::issueWrite - Protocol Error (no record layer reference). Name=%s", m_name() );
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
        Cpl::System::FatalError::logf( "Rte::Db::Record::Base::tellInitialized - Protocol Error (no record layer reference). Name=%s", m_name() );
        }

    if ( !m_missingTuples )
        {
        m_recLayerPtr->notifyRecordInitialized();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellInitialized [%s] -->good load", m_name()) );
        }
    else
        {
        m_recLayerPtr->notifyRecordConverted();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellInitialized [%s] -->missing tuples, aka auto minor upgrade", m_name()) );
        }
    }

void Base::tellStartCompleted() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStartCompleted [%s]", m_name()) );
    if ( !m_recLayerPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Base::tellStartCompleted - Protocol Error (no record layer reference). Name=%s", m_name() );
        }

    m_recLayerPtr->notifyRecordStarted();
    }

void Base::tellStarting() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStarting [%s]", m_name()) );
    if ( !m_recLayerPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Base::tellStarting - Protocol Error (no record layer reference). Name=%s", m_name() );
        }

    m_recLayerPtr->notifyRecordWaiting();
    }

void Base::tellStopped() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Base::tellStopped [%s]", m_name()) );
    if ( !m_recLayerPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Base::tellStopped - Protocol Error (no record layer reference). Name=%s", m_name() );
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


