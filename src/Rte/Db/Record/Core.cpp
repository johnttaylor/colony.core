/*-----------------------------------------------------------------------------
* This file is part of the Shift-Right Open Repository.  The Repository is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the root directory or on the Internet at
* http://www.shift-right.com/openrepo/license.htm
*
* Copyright (c) 2001-2010 John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Core.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Log/Api.h"
#include "Rte/Db/Framing.h"
#include <string.h>
#include <new>

using namespace Rte::Db::Record;

#define SECT_   "Rte::Db::Record"

/////////////////////////////////
Core::Core( uint8_t*                      recordLayerBuffer,
            uint32_t                      bufferSize,
            Client&                       setLayerHandler,
            Rte::Db::Chunk::Request::SAP& chunkSAP,
            Cpl::Itc::PostApi&            recordLayerMbox,
            Cpl::Log::Api&                eventLogger,
            ErrorClient*                  errorHandler
          )
:m_buffer(recordLayerBuffer)
,m_bufSize(bufferSize)
,m_setLayer(setLayerHandler)
,m_chunkSAP(chunkSAP)
,m_myMbox(recordLayerMbox)
,m_dbResult(Rte::Db::Chunk::Request::eSUCCESS)
,m_dbDataLen(0)
,m_recordPtr(0)
,m_queuedEvent(FSM_NO_MSG)
,m_logger(eventLogger)
,m_errHandlerPtr(errorHandler)
,m_closeDbResponseMsg(*this, recordLayerMbox, chunkSAP, m_closeDbPayload)
    {
    // Initialize my FSM
    initialize();
    }


/////////////////////////////////
void Core::start( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::start") );
    
    // Generate a Fatal Error if NOT in the Idle state
    if ( getInnermostActiveState() != Idle )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Core::start().  Protocol error.  The FSM is NOT in the idle state (currentState=%s)", getNameByState(getInnermostActiveState()) );
        }
        
    // WORKAROUND: Reset my FSM history on the start event because I can't reset 
    //             it from a FSM action due to how the auto-generated code works
    resetHistoryActive();
    resetHistoryOpening();

    // Send the start event
    sendEvent( evStart ); 
    }

void Core::stop( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::stop") );
    sendEvent( evStop ); 
    }


void Core::write( Api& recordToWrite )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::write") );
    m_writeRequests.put( recordToWrite );
    sendEvent( evWrite ); 
    }


/////////////////////////////////
void Core::response( OpenDbMsg& msg )
    {
    m_dbResult = msg.getRequestMsg().getPayload().m_result;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::response( OpenDbMsg& msg ): result=%s", Rte::Db::Chunk::Request::resultToString(m_dbResult)) );

    sendEvent( evResponse ); 
    }

void Core::response( CloseDbMsg& msg )
    {
    m_dbResult = msg.getRequestMsg().getPayload().m_result;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::response( CloseDbMsg& msg ): result=%s", Rte::Db::Chunk::Request::resultToString(m_dbResult)) );

    sendEvent( evStopped ); 
    }

void Core::response( ClearDbMsg& msg )
    {
    m_dbResult = msg.getRequestMsg().getPayload().m_result;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::response( ClearDbMsg& msg ): result=%s", Rte::Db::Chunk::Request::resultToString(m_dbResult)) );

    sendEvent( evResponse ); 
    }

void Core::response( ReadMsg& msg )
    {
    m_dbResult  = msg.getRequestMsg().getPayload().m_result;
    m_dbDataLen = msg.getRequestMsg().getPayload().m_handlePtr->m_len;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::response( ReadMsg& msg ): result=%s, inLen=%lu", Rte::Db::Chunk::Request::resultToString(m_dbResult), m_dbDataLen));

    sendEvent( evResponse ); 
    }

void Core::response( WriteMsg& msg )
    {
    m_dbResult  = msg.getRequestMsg().getPayload().m_result;
    m_dbDataLen = msg.getRequestMsg().getPayload().m_handlePtr->m_len;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::response( WriteMsg& msg ): result=%s, outLen=%lu", Rte::Db::Chunk::Request::resultToString(m_dbResult), m_dbDataLen) );

    sendEvent( evResponse ); 
    }


/////////////////////////////////
void Core::reportIncompatible() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::reportIncompatible") );
    m_logger.warning( "Rte::Db::Record::Core::reportIncompatible - Incompatible schema" );
    if ( m_errHandlerPtr ) 
        {
        m_errHandlerPtr->reportDbIncompatible();
        }
    }

void Core::reportFileReadError() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::reportFileReadError") );
    m_logger.warning( "Rte::Db::Record::Core::reportFileReadError - DB File Read/Open error" );
    if ( m_errHandlerPtr ) 
        {
        m_errHandlerPtr->reportDbFileOpenError( m_dbResult );
        }
    }

void Core::reportDataCorruptError() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::reportDataCorruptError") );
    m_logger.warning( "Rte::Db::Record::Core::reportDataCorruptError - DB Corruption error" );
    if ( m_errHandlerPtr ) 
        {
        m_errHandlerPtr->reportDbCorruptionError();
        }
    }

void Core::reportFileWriteError() throw()
    {
    const char* recname = m_recordPtr->getName();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::reportFileWriteError (rec=%s)", recname) );
    m_logger.warning( "Rte::Db::Record::Core::reportFileWriteError - DB File Write error (rec=%s)", recname  );
    if ( m_errHandlerPtr ) 
        {
        m_errHandlerPtr->reportDbFileWriteError( recname );
        }
    }



void Core::requestDbClose() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::requestDbClose") );
    m_chunkSAP.post( m_closeDbResponseMsg.getRequestMsg() );
    }

void Core::requestDbOpen() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::requestDbOpen") );
    if ( m_errHandlerPtr ) 
        {
        m_errHandlerPtr->reportDbOpened();
        }

    Rte::Db::Chunk::Request::OpenDbPayload* payload = new(m_memPayload.m_byteMem)      Rte::Db::Chunk::Request::OpenDbPayload(m_buffer, m_bufSize);
    Rte::Db::Chunk::Response::OpenDbMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Rte::Db::Chunk::Response::OpenDbMsg(*this, m_myMbox, m_chunkSAP, *payload);      
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    }

void Core::requestDbClear() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::requestDbClear") );
    Rte::Db::Chunk::Request::ClearDbPayload* payload = new(m_memPayload.m_byteMem)      Rte::Db::Chunk::Request::ClearDbPayload(m_buffer, m_bufSize);
    Rte::Db::Chunk::Response::ClearDbMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Rte::Db::Chunk::Response::ClearDbMsg(*this, m_myMbox, m_chunkSAP, *payload);      
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    }

void Core::requestDbRead() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::requestDbRead") );
    Rte::Db::Chunk::Request::ReadPayload* payload = new(m_memPayload.m_byteMem)      Rte::Db::Chunk::Request::ReadPayload(m_buffer, m_bufSize, m_chunkHandle);
    Rte::Db::Chunk::Response::ReadMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Rte::Db::Chunk::Response::ReadMsg(*this, m_myMbox, m_chunkSAP, *payload);      
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    m_dbDataLen = 0;
    }

void Core::requestDbWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::requestDbWrite") );
    m_recordPtr = m_writeRequests.get();
    if ( !m_recordPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Core::requestDbWrite - Protocol Error." );
        }

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::requestDbWrite - writing recordName=%s", m_recordPtr->getName()) );
    plantRecName( m_recordPtr->getName() );
    uint32_t dataLen = m_recordPtr->fillWriteBuffer( extractDataPointer(), maxAllowedDataSize() );
        
    Rte::Db::Chunk::Request::WritePayload* payload = new(m_memPayload.m_byteMem)      Rte::Db::Chunk::Request::WritePayload(m_buffer, calcRecordLen(dataLen), m_recordPtr->getChunkHandle());
    Rte::Db::Chunk::Response::WriteMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Rte::Db::Chunk::Response::WriteMsg(*this, m_myMbox, m_chunkSAP, *payload);      
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    }

void Core::consumeNoWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::consumeNoWrite") );
    m_recordPtr = m_writeRequests.get();
    if ( !m_recordPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Core::consumeNoWrite - Protocol Error." );
        }
    }



void Core::ackRead() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::ackRead") );
    Api* recordP = m_setLayer.getRecordApi( extractRecName(), extractRecNameLength() );
    if ( recordP )
        {
        recordP->setChunkHandle( m_chunkHandle );
        recordP->notifyRead( extractDataPointer(), calcDataLength() );
        }
    }

void Core::ackWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::ackWrite") );
    if ( m_recordPtr )
        {
        m_recordPtr->notifyWriteDone();
        m_recordPtr = 0;
        }
    else
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Core::ackWrite - Protocol Error." );
        }
    }

void Core::nakWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::nakWrite") );
    m_setLayer.notifyWriteError();
    }


void Core::ackOpenDone() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::ackOpenDone") );
    m_setLayer.notifyOpenCompleted();
    }

void Core::nakOpenDone() throw()
    {
    Client::OpenError_T status;

    // Translate response code to my client open_error code
    if ( m_dbResult == Rte::Db::Chunk::Request::eEOF )
        {
        status = Client::eMISSING_SETS;
        }     
    else if ( m_dbResult == Rte::Db::Chunk::Request::eCORRUPT_DATA )
        {
        status = Client::eCORRUPT_DATA;
        }
    else if ( m_dbResult == Rte::Db::Chunk::Request::eWRONG_SCHEMA )
        {
        status = Client::eERR_WRONG_SCHEMA;
        }
    else 
        {
        status = Client::eERR_MEDIA;
        }


    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::nakOpenDone. OpenError_T=%u", status ) );
    m_setLayer.notifyOpenedWithErrors( status );
    }

void Core::ackDbStopped() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::ackDbStopped") );
    m_setLayer.notifyStopped();
    if ( m_errHandlerPtr ) 
        {
        m_errHandlerPtr->reportDbClosed();
        }
    }



void Core::queWriteRequest() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::queWriteRequest") );
    // I do nothing since I ALWAYS queues the incoming write requests
    }

void Core::inspectWriteQue() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::inspectWriteQue") );
    if ( m_writeRequests.first() )
        {
        generateInternalEvent( evWrite ); 
        }
    }

void Core::clearWriteQue() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::clearWriteQue") );
    m_writeRequests.clearTheList();
    }

void Core::verifyOpen() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Core::verifyOpen") );
    if ( m_setLayer.isCleanLoad() )
        {
        generateInternalEvent( evVerified ); 
        }
    else
        {
        generateInternalEvent( evIncompleteLoad ); 
        }
    }



/////////////////////////////////
bool Core::isDbEof() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eEOF;
    }

bool Core::isDbSuccess() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eSUCCESS;
    }

bool Core::isDbBadData() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eCORRUPT_DATA;
    }

bool Core::isDbError() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eERR_FILEIO || m_dbResult == Rte::Db::Chunk::Request::eERR_NOT_A_DB_FILE || m_dbResult == Rte::Db::Chunk::Request::eERR_OPEN;
    }

bool Core::isNotCompatible() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eWRONG_SCHEMA;
    }


/////////////////////////////////
void Core::sendEvent( FSM_EVENT_T msg )
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

void Core::generateInternalEvent( FSM_EVENT_T msg )
    {
    m_queuedEvent = msg;
    }

const char* Core::extractRecName(void)
    {
    return (const char*)(m_buffer + Rte::Db::eNLEN_SIZE);
    }

uint16_t Core::extractRecNameLength(void)
    {
    return *((uint16_t*)m_buffer);
    }

uint32_t Core::calcDataLength(void)
    {
    return m_dbDataLen - (Rte::Db::eNLEN_SIZE + extractRecNameLength());
    }

uint8_t* Core::extractDataPointer(void)
    {
    return m_buffer + Rte::Db::eNLEN_SIZE + extractRecNameLength();
    }

uint32_t Core::maxAllowedDataSize(void)
    {
    return m_bufSize - (Rte::Db::eNLEN_SIZE + extractRecNameLength());
    }

void Core::plantRecName( const char* recName )
    {
    uint16_t nameLen = (uint16_t) strlen(recName);
    *((uint16_t*)m_buffer) = nameLen;
    memcpy( m_buffer + Rte::Db::eNLEN_SIZE, recName, nameLen );
    }

uint32_t Core::calcRecordLen( uint32_t datalen )
    {
    return datalen + Rte::Db::eNLEN_SIZE + extractRecNameLength();
    }
