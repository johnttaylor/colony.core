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

#include "Server.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Log/Api.h"
#include "Rte/Db/Framing.h"
#include <string.h>
#include <new>

using namespace Rte::Db::Record;

#define SECT_   "Rte::Db::Record"

/////////////////////////////////
Server::Server( uint8_t*                       recordLayerBuffer,
                uint32_t                       bufferSize,
                Rte::Db::Chunk::Request::SAP&  chunkSAP,
                Cpl::Itc::PostApi&             recordLayerMbox,
                Cpl::Container::Map<ApiLocal>& recordList,
                Cpl::Log::Api&                 eventLogger
              )
:Cpl::Itc::CloseSync(recordLayerMbox)
,m_healthSAP(*this, recordLayerMbox) 
,m_recordLayer(recordLayer)
,m_conversion(false)
,m_recordCount(0)
,m_openCount(0)
,m_closeCount(0)
,m_openMsgPtr(0)
,m_closeMsgPtr(0)
,m_status( HealthRequest::eCLOSED )
,m_tempStatus( m_status )
,m_buffer(recordLayerBuffer)
,m_bufSize(bufferSize)
,m_chunkSAP(chunkSAP)
,m_myMbox(recordLayerMbox)
,m_dbResult(Rte::Db::Chunk::Request::eSUCCESS)
,m_dbDataLen(0)
,m_writerPtr(0)
,m_logger(eventLogger)
,m_records(recordList)
,m_closeDbResponseMsg(*this, recordLayerMbox, chunkSAP, m_closeDbPayload)
    {
    // Initialize my FSM
    initialize();
    }


/////////////////////////////////
HealthRequest::SAP& Server::getHealthSAP(void)
    {
    return m_healthSAP;
    }

void Server::request( HealthMsg& msg )
    {
    // Return immediately if there is a difference in ASSUMED status vs actual
    if ( msg.getPayload().m_status != m_status )
        {
        msg.getPayload().m_status = m_status;
        msg.returnToSender();
        }

    // Que notification request until there is change
    else
        {
        m_pendingHealth.put( msg );
        }
    }


void Server::defaultAllRecordsContent() throw()
    {
    DefaultPayload                payload;
    Cpl::Itc::SyncReturnHandler   srh;
    DefaultMsg                    msg(*this,payload,srh);
    m_mbox.postSync(msg);
    }


void Server::request( DefaultMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::DefaultMsg (opened=%d", m_opened ) );

    // Ignore request if NOT opened, i.e. it would do anything anyway since all 
    // Sets are defaulted as part of the opening the DB sequence
    if ( m_opened )
        {
        // Default ALL registered sets
        ApiLocal* recordPtr = m_records.first();
        while( recordPtr )
            {
            recordPtr->defaultContent();
            recordPtr = m_records.next( *recordPtr );
            }
        }

    msg.returnToSender();
    }

void Server::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::OpenMsg") );

    // Generate a Fatal Error if NOT in the Idle state
    if ( getInnermostActiveState() != Idle )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Server::start().  Protocol error.  The FSM is NOT in the idle state (currentState=%s)", getNameByState(getInnermostActiveState()) );
        }
        
    // Housekeeping
    m_conversion    = false;
    m_recordCount   = 0;
    m_openCount     = 0;
    m_openMsgPtr    = &msg;
    m_tempStatus    = HealthRequest::eOPENING; 
    setNewHealthStatus( m_tempStatus );

    // Start all registered records 
    ApiLocal* recordPtr = m_records.first();
    while( recordPtr )
        {
        m_openCount++;
        recordPtr->start( *this );
        recordPtr = m_records.next( *recordPtr );
        }
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::OpenMsg - record count=%d", m_openCount) );


    // Start the Record Handler
    // WORKAROUND: Reset my FSM history on the start event because I can't reset 
    //             it from a FSM action due to how the auto-generated code works
    resetHistoryActive();
    resetHistoryOpening();

    // Send the start event
    generateEvent( evStart ); 
    }

void Server::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::CloseMsg") );

    // Something is BAD if trying to close an already closed DB
    if ( getInnermostActiveState() == Idle )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Server::request::CloseMsg - Protocol error. The FSM is IN the idle state." );
        }

    // Housekeeping
    m_closeCount  = 0;
    m_closeMsgPtr = &msg;

    // Stop all registered Sets
    Rte::Db::Record::ApiLocal* recordPtr = m_records.first();
    while( recordPtr )
        {
        m_closeCount++;
        recordPtr->stop();
        recordPtr = m_records.next( *recordPtr );
        }
    }

/////////////////////////////////
void Server::write( ApiLocalWriter& recordToWrite )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::write") );

    void* notEmpty = m_writeRequests.first();
    m_writeRequests.put( recordToWrite );

    // The nominal path is for the inspectWriteQue() action to generate ALL of the
    // evWrite events so that my HandlerFsmEventQueue does NOT have to be unbounded in 
    // size (i.e. dependent on the number Records in the application).  However, this
    // paradigm breaks down when the FSM is in the 'Writeable' state -->hence the
    // following hack.
    if ( !notEmpty && getInnermostActiveState() == Writeable )
        {
        generateEvent( evWrite );   
        }
    }


void Server::notifyRecordWaiting( ApiLocal& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordWaiting") );
    m_recordCount++;
    }

void Server::notifyRecordInitialized( ApiLocal& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordInitialized") );
    m_recordCount--;
    }

void Server::notifyRecordConverted( ApiLocal& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordConverted") );
    m_conversion = true;
    }

void Server::notifyRecordStopped( ApiLocal& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordStopped") );

    // Something is BAD if my record is stopped and the Record Handler has NOT be requested to stop
    if ( !m_closeMsgPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Server::notifyRecordStopped - Protocol error." );
        }

    // Stop the Record Handler ONCE ALL Records have stopped
    if ( --m_closeCount == 0 )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::recordStopped - Record Layer Stopped") );
        generateEvent( evStop ); 
        }
    }
    
void Server::notifyRecordStarted( ApiLocal& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordStarted. openCount=%d", m_openCount) );

    // Something is BAD if a Record is opening and the Record Handler has NOT be requested to open
    if ( !m_openMsgPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Server::notifyRecordStarted - Protocol error." );
        }

    // Return my Open Request message once ALL records have completed their opening sequence.
    if ( --m_openCount == 0 )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordStarted - Record Layer Started") );
        
        // Update my health (and mark the openMsg as failed when appropriate)    
        if ( m_tempStatus == HealthRequest::eOPENING )
            {
            setNewHealthStatus( HealthRequest::eRUNNING );
            }
        else
            {
            m_openMsgPtr->getPayload().m_success = false;
            setNewHealthStatus( m_tempStatus );
            }

        // Return the OpenMsg now that ALL Records are 'opened'
        m_openMsgPtr->returnToSender();
        m_openMsgPtr = 0;
        }
    }


/////////////////////////////////
void Server::response( OpenDbMsg& msg )
    {
    m_dbResult = msg.getRequestMsg().getPayload().m_result;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( OpenDbMsg& msg ): result=%s", Rte::Db::Chunk::Request::resultToString(m_dbResult)) );

    generateEvent( evResponse ); 
    }

void Server::response( CloseDbMsg& msg )
    {
    m_dbResult = msg.getRequestMsg().getPayload().m_result;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( CloseDbMsg& msg ): result=%s", Rte::Db::Chunk::Request::resultToString(m_dbResult)) );

    generateEvent( evStopped ); 
    }

void Server::response( ClearDbMsg& msg )
    {
    m_dbResult = msg.getRequestMsg().getPayload().m_result;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( ClearDbMsg& msg ): result=%s", Rte::Db::Chunk::Request::resultToString(m_dbResult)) );

    generateEvent( evResponse ); 
    }

void Server::response( ReadMsg& msg )
    {
    m_dbResult  = msg.getRequestMsg().getPayload().m_result;
    m_dbDataLen = msg.getRequestMsg().getPayload().m_handlePtr->m_len;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( ReadMsg& msg ): result=%s, inLen=%lu", Rte::Db::Chunk::Request::resultToString(m_dbResult), m_dbDataLen));

    generateEvent( evResponse ); 
    }

void Server::response( WriteMsg& msg )
    {
    m_dbResult  = msg.getRequestMsg().getPayload().m_result;
    m_dbDataLen = msg.getRequestMsg().getPayload().m_handlePtr->m_len;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( WriteMsg& msg ): result=%s, outLen=%lu", Rte::Db::Chunk::Request::resultToString(m_dbResult), m_dbDataLen) );

    generateEvent( evResponse ); 
    }


/////////////////////////////////
void Server::reportIncompatible() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportIncompatible") );
    m_logger.warning( "Rte::Db::Record::Server::reportIncompatible - Incompatible schema" );
    m_tempStatus = HealthRequest::eNO_STORAGE_WRONG_SCHEMA;
    }

void Server::reportFileReadError() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportFileReadError") );
    m_logger.warning( "Rte::Db::Record::Server::reportFileReadError - DB File Read/Open error" );
    m_tempStatus = HealthRequest::eNO_STORAGE_MEDIA_ERR;
    }

void Server::reportDataCorruptError() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportDataCorruptError") );
    m_logger.warning( "Rte::Db::Record::Server::reportDataCorruptError - DB Corruption error - defaulting one or more record(s)" );
    }

void Server::reportFileWriteError() throw()
    {
    const char* recname = m_writerPtr->getName();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportFileWriteError (rec=%s)", recname) );
    m_logger.warning( "Rte::Db::Record::Server::reportFileWriteError - DB File Write error (rec=%s)", recname  );
    m_tempStatus = HealthRequest::eNO_STORAGE_MEDIA_ERR;
    }



void Server::requestDbClose() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestDbClose") );
    m_chunkSAP.post( m_closeDbResponseMsg.getRequestMsg() );
    }

void Server::requestDbOpen() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestDbOpen") );
    Rte::Db::Chunk::Request::OpenDbPayload* payload = new(m_memPayload.m_byteMem)      Rte::Db::Chunk::Request::OpenDbPayload(m_buffer, m_bufSize);
    Rte::Db::Chunk::Response::OpenDbMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Rte::Db::Chunk::Response::OpenDbMsg(*this, m_myMbox, m_chunkSAP, *payload);      
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    }

void Server::requestDbClear() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestDbClear") );
    Rte::Db::Chunk::Request::ClearDbPayload* payload = new(m_memPayload.m_byteMem)      Rte::Db::Chunk::Request::ClearDbPayload(m_buffer, m_bufSize);
    Rte::Db::Chunk::Response::ClearDbMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Rte::Db::Chunk::Response::ClearDbMsg(*this, m_myMbox, m_chunkSAP, *payload);      
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    }

void Server::requestDbRead() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestDbRead") );
    Rte::Db::Chunk::Request::ReadPayload* payload = new(m_memPayload.m_byteMem)      Rte::Db::Chunk::Request::ReadPayload(m_buffer, m_bufSize, m_chunkHandle);
    Rte::Db::Chunk::Response::ReadMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Rte::Db::Chunk::Response::ReadMsg(*this, m_myMbox, m_chunkSAP, *payload);      
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    m_dbDataLen = 0;
    }

void Server::requestDbWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestDbWrite") );
    m_writerPtr = m_writeRequests.get();
    if ( !m_writerPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Server::requestDbWrite - Protocol Error." );
        }

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestDbWrite - writing recordName=%s", m_writerPtr->getName()) );
    plantRecName( m_writerPtr->getName() );
    uint32_t dataLen = m_writerPtr->fillWriteBuffer( extractDataPointer(), maxAllowedDataSize() );
        
    Rte::Db::Chunk::Request::WritePayload* payload = new(m_memPayload.m_byteMem)      Rte::Db::Chunk::Request::WritePayload(m_buffer, calcRecordLen(dataLen), m_writerPtr->getChunkHandle());
    Rte::Db::Chunk::Response::WriteMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Rte::Db::Chunk::Response::WriteMsg(*this, m_myMbox, m_chunkSAP, *payload);      
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    }


void Server::consumeNoWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::consumeNoWrite") );
    m_writerPtr = m_writeRequests.get();
    if ( !m_writerPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Server::consumeNoWrite - Protocol Error." );
        }
    }


void Server::ackRead() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackRead") );
    ApiLocal* recordPtr = findRecordApi( extractRecName(), extractRecNameLength() );
    if ( recordPtr )
        {
        recordPtr->setChunkHandle( m_chunkHandle );
        if ( recordPtr->notifyRead( extractDataPointer(), calcDataLength() ) )
            {
            // Succesfull read --> start the read of the next record
            requestDbRead();
            }

        // Failed read!
        else
            {
            // Log the failure
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackRead - Record read failed, i.e. incompatible DB") );
            m_logger.warning( "Rte::Db::Record::Server::ackRead - Record read failed, i.e. incompatible DB" );

            // Send myself an 'incompatible' event to transition to the NO-Persistence state
            m_dbResult = Rte::Db::Chunk::Request::eWRONG_SCHEMA;
            generateEvent( evResponse ); 
            }
        }
    }

void Server::ackWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackWrite") );
    if ( m_writerPtr )
        {
        m_writerPtr->notifyWriteDone();
        m_writerPtr = 0;
        }
    else
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Server::ackWrite - Protocol Error." );
        }
    }

void Server::nakWrite() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::nakWrite") );
    m_writerPtr = 0;
    }


void Server::ackOpenDone() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackOpenDone") );

    ApiLocal* recordPtr = m_records.first();
    while( recordPtr )
        {
        recordPtr->notifyLoadCompleted();
        recordPtr = m_records.next( *recordPtr );
        }
    }


void Server::nakOpenDone() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::nakOpenDone") );

    ApiLocal* recordPtr = m_records.first();
    while( recordPtr )
        {
        recordPtr->notifyLoadFailed();
        recordPtr = m_records.next( *recordPtr );
        }
    }


void Server::ackDbStopped() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackDbStopped") );

    // Something is BAD if the Record Handler has stopped and the Record Handler has NOT be requested to stop
    if ( !m_closeMsgPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Record::Server::ackDbStopped - Protocol error." );
        }

    // Return/complete my close request
    setNewHealthStatus( HealthRequest::eCLOSED );      // Return Health Request Msg BEFORE the close message -->this makes clean-up on the client easier.
    m_closeMsgPtr->returnToSender();
    m_closeMsgPtr = 0;
    }

void Server::queWriteRequest() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::queWriteRequest") );
    // Nothing needed!
    }

void Server::inspectWriteQue() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::inspectWriteQue") );
    
    // Generate a write event if there is NOT a write an progress AND there is at least one queued write request
    if ( m_writerPtr == 0 && m_writeRequests.first() )
        {
        generateEvent( evWrite ); 
        }
    }

void Server::clearWriteQue() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::clearWriteQue") );
    m_writeRequests.clearTheList();
    m_writerPtr = 0;
    }

void Server::verifyOpen() throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::verifyOpen") );
    if ( m_recordCount == 0 && !m_conversion )
        {
        generateEvent( evVerified ); 
        }
    else
        {
        generateEvent( evIncompleteLoad ); 
        }
    }



/////////////////////////////////
bool Server::isDbEof() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eEOF;
    }

bool Server::isDbSuccess() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eSUCCESS;
    }

bool Server::isDbBadData() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eCORRUPT_DATA;
    }

bool Server::isDbError() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eERR_FILEIO || m_dbResult == Rte::Db::Chunk::Request::eERR_NOT_A_DB_FILE || m_dbResult == Rte::Db::Chunk::Request::eERR_OPEN;
    }

bool Server::isNotCompatible() throw()
    {
    return m_dbResult == Rte::Db::Chunk::Request::eWRONG_SCHEMA;
    }


/////////////////////////////////
const char* Server::extractRecName(void)
    {
    return (const char*)(m_buffer + Rte::Db::eNLEN_SIZE);
    }

uint16_t Server::extractRecNameLength(void)
    {
    return *((uint16_t*)m_buffer);
    }

uint32_t Server::calcDataLength(void)
    {
    return m_dbDataLen - (Rte::Db::eNLEN_SIZE + extractRecNameLength());
    }

uint8_t* Server::extractDataPointer(void)
    {
    return m_buffer + Rte::Db::eNLEN_SIZE + extractRecNameLength();
    }

uint32_t Server::maxAllowedDataSize(void)
    {
    return m_bufSize - (Rte::Db::eNLEN_SIZE + extractRecNameLength());
    }

void Server::plantRecName( const char* recName )
    {
    uint16_t nameLen = (uint16_t) strlen(recName);
    *((uint16_t*)m_buffer) = nameLen;
    memcpy( m_buffer + Rte::Db::eNLEN_SIZE, recName, nameLen );
    }

uint32_t Server::calcRecordLen( uint32_t datalen )
    {
    return datalen + Rte::Db::eNLEN_SIZE + extractRecNameLength();
    }


/////////////////////////////////
void Server::setNewHealthStatus( HealthRequest::Status_T newstatus )
    {
    // Update my status
    m_status = newstatus;

    // Generate change notification(s)
    HealthMsg* ptr = m_pendingHealth.first();
    while( ptr )
        {
        HealthMsg* nextPtr = m_pendingHealth.next( *ptr );

        if ( ptr->getPayload().m_status != m_status )
            {
            m_pendingHealth.remove( *ptr );
            ptr->getPayload().m_status = m_status;
            ptr->returnToSender();
            }

        ptr = nextPtr;
        }
    }


ApiLocal* Server::findRecordApi( const char* recName, uint16_t nameLen )
    {
    Cpl::Container::KeyStringBuffer recordNameKey( recName, nameLen );
    return m_records.find( recordNameKey );
    }
