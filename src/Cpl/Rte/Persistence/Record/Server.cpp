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
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/Rte/Persistence/Framing.h"
#include "Cpl/Text/FString.h"
#include <string.h>
#include <new>

using namespace Cpl::Rte::Persistence::Record;

#define SECT_   "Cpl::Rte::Persistence::Record"

/////////////////////////////////
Server::Server( uint8_t*                                    recordLayerBuffer,
                uint32_t                                    bufferSize,
                Cpl::Rte::Persistence::Chunk::Request::SAP& chunkSAP,
                Cpl::Rte::MailboxServer&                    recordLayerMbox,
                Cpl::Container::Map<Api_>&                  recordList,
                MpServerStatus&                             serverStatusMp,
                Cpl::Rte::Mp::String&                       defaultRequestMp,
                Cpl::Log::Api&                              eventLogger )
    :ServerApi( recordLayerMbox )
    , Subscriber<Cpl::Rte::Mp::String>( recordLayerMbox )
    , m_conversion( false )
    , m_totalRecordCount( 0 )
    , m_recordCount( 0 )
    , m_openCount( 0 )
    , m_closeCount( 0 )
    , m_openMsgPtr( 0 )
    , m_closeMsgPtr( 0 )
    , m_statusMp( serverStatusMp )
    , m_localStatus( ServerStatus::eCLOSED )
    , m_defaultRequestMp( defaultRequestMp )
    , m_buffer( recordLayerBuffer )
    , m_bufSize( bufferSize )
    , m_chunkSAP( chunkSAP )
    , m_myMbox( recordLayerMbox )
    , m_fileResult( Cpl::Rte::Persistence::Chunk::ServerResult::eSUCCESS )
    , m_fileDataLen( 0 )
    , m_writerPtr( 0 )
    , m_logger( eventLogger )
    , m_records( recordList )
    , m_closeFileResponseMsg( *this, recordLayerMbox, chunkSAP, m_closeFilePayload )
{
    // Initialize my FSM
    initialize();

    // Update my status
    m_statusMp.write( m_localStatus );
}

///////////////////////////////////////
void Server::modelPointChanged( Cpl::Rte::Mp::String& modelPointThatChanged ) throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::modelPointChanged") );

    Cpl::Text::FString<OPTION_CPL_RTE_PERSISTENCE_RECORD_MAX_RECORD_NAME_LEN> name;
    int8_t mpState;
    modelPointThatChanged.read( name, mpState );
    if ( Cpl::Rte::ModelPoint::IS_VALID( mpState ) )
    {
        // Default ALL records
        if ( name == "*" )
        {
            defaultAllRecords();
        }

        // Default a single record
        else
        {
            Api_* recordPtr = m_records.find( name );
            if ( recordPtr )
            {
                recordPtr->defaultContent();
            }
            else
            {
                m_logger.warning( "Cpl::Rte::Persistence::Record::Server.  Requested to default the record: %s - but the record does not exist", name.getString() );
            }
        }

        // Clear the request (but do in any that I don't get a change notifcation for the clear action)
        modelPointThatChanged.detach( *this );
        uint16_t seqNum = modelPointThatChanged.write( "" );
        modelPointThatChanged.attach( *this, seqNum );
    }
}

///////////////////////////////////////
void Server::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::OpenMsg") );

    // Generate a Fatal Error if NOT in the Idle state
    if ( getInnermostActiveState() != Idle )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Server::start().  Protocol error.  The FSM is NOT in the idle state (currentState=%s)", getNameByState( getInnermostActiveState() ) );
    }

    // Housekeeping
    m_conversion       = false;
    m_totalRecordCount = 0;
    m_recordCount      = 0;
    m_openMsgPtr       = &msg;
    m_localStatus      = ServerStatus::eOPENING;
    m_statusMp.write( m_localStatus );

    // Determine the number of registered records
    Api_* recordPtr = m_records.first();
    while ( recordPtr )
    {
        m_totalRecordCount++;
        recordPtr = m_records.next( *recordPtr );
    }
    m_openCount  = m_totalRecordCount;
    m_closeCount = m_totalRecordCount;

    // Start all registered records (MUST be done AFTER I have obtained my total record count) 
    recordPtr = m_records.first();
    while ( recordPtr )
    {
        recordPtr->start( *this );
        recordPtr = m_records.next( *recordPtr );
    }

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::OpenMsg - record count=%d", m_totalRecordCount) );


    // Start the Record Handler
    // WORKAROUND: Reset my FSM history on the start event because I can't reset 
    //             it from a FSM action due to how the auto-generated code works
    resetHistoryActive();
    resetHistoryOpening();

    // Send the start event
    generateEvent( HandlerFsm_evStart );
}

void Server::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::CloseMsg. m_closeCount=%d", m_closeCount) );

    // Something is BAD if trying to close an already closed DB
    if ( getInnermostActiveState() == Idle )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Server::request::CloseMsg - Protocol error. The FSM is IN the idle state." );
    }

    // Housekeeping
    m_closeMsgPtr = &msg;
    m_statusMp.write( ServerStatus::eCLOSING );
    m_defaultRequestMp.detach( *this );

    // Stop all registered Records
    Api_* recordPtr = m_records.first();
    while ( recordPtr )
    {
        recordPtr->stop();
        recordPtr = m_records.next( *recordPtr );
    }

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::CloseMsg. m_closeCount=%d", m_closeCount) );
}

/////////////////////////////////
void Server::write( ApiWriter_& recordToWrite )
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
        generateEvent( HandlerFsm_evWrite );
    }
}


void Server::notifyRecordWaiting( void )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordWaiting") );
    m_recordCount++;
}

void Server::notifyRecordInitialized( void )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordInitialized") );
    m_recordCount--;
}

void Server::notifyRecordConverted( void )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordConverted") );
    m_conversion = true;
}

void Server::notifyRecordStopped( void )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordStopped. m_closeCount=%d", m_closeCount) );

    // Something is BAD if my record is stopped and the Record Handler has NOT be requested to stop
    if ( !m_closeMsgPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Server::notifyRecordStopped - Protocol error." );
    }

    // Stop the Record Handler ONCE ALL Records have stopped
    if ( --m_closeCount == 0 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::recordStopped - Record Layer Stopped") );
        generateEvent( HandlerFsm_evStop );
    }
}

void Server::notifyRecordStarted( void )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordStarted. openCount=%d", m_openCount) );

    // Something is BAD if a Record is opening and the Record Handler has NOT be requested to open
    if ( !m_openMsgPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Server::notifyRecordStarted - Protocol error." );
    }

    // Return my Open Request message once ALL records have completed their opening sequence.
    if ( --m_openCount == 0 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifyRecordStarted - Record Layer Started") );

        // Update my health (and mark the openMsg as failed when appropriate)   
        if ( m_localStatus == +ServerStatus::eOPENING )
        {
            m_localStatus = ServerStatus::eRUNNING;
        }
        else
        {
            // Set error result to the open request if in 'Non-Perstistance' mode
            if ( m_localStatus == +ServerStatus::eNO_STORAGE_MEDIA_ERR || m_localStatus == +ServerStatus::eNO_STORAGE_WRONG_SCHEMA )
            {
                m_openMsgPtr->getPayload().m_success = false;
            }
        }

        // Update my status
        m_statusMp.write( m_localStatus );

        // Register for change notifications on the 'Default Request Action' MP
        m_defaultRequestMp.attach( *this );

        // Return the OpenMsg now that ALL Records are 'opened'
        m_openMsgPtr->returnToSender();
        m_openMsgPtr = 0;
    }
}


/////////////////////////////////
void Server::response( OpenFileMsg& msg )
{
    m_fileResult = msg.getRequestMsg().getPayload().m_result;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( OpenFileMsg& msg ): result=%s", m_fileResult._to_string()) );

    generateEvent( HandlerFsm_evResponse );
}

void Server::response( CloseFileMsg& msg )
{
    m_fileResult = msg.getRequestMsg().getPayload().m_result;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( CloseFileMsg& msg ): result=%s", m_fileResult._to_string()) );

    generateEvent( HandlerFsm_evStopped );
}

void Server::response( ClearFileMsg& msg )
{
    m_fileResult = msg.getRequestMsg().getPayload().m_result;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( ClearFileMsg& msg ): result=%s", m_fileResult._to_string()) );

    generateEvent( HandlerFsm_evResponse );
}

void Server::response( ReadFileMsg& msg )
{
    m_fileResult  = msg.getRequestMsg().getPayload().m_result;
    m_fileDataLen = msg.getRequestMsg().getPayload().m_handlePtr->m_len;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( ReadMsg& msg ): result=%s, inLen=%lu", m_fileResult._to_string(), m_fileDataLen) );

    generateEvent( HandlerFsm_evResponse );
}

void Server::response( WriteFileMsg& msg )
{
    m_fileResult  = msg.getRequestMsg().getPayload().m_result;
    m_fileDataLen = msg.getRequestMsg().getPayload().m_handlePtr->m_len;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::response( WriteMsg& msg ): result=%s, outLen=%lu", m_fileResult._to_string(), m_fileDataLen) );

    generateEvent( HandlerFsm_evResponse );
}


/////////////////////////////////
void Server::reportIncompatible() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportIncompatible") );
    m_logger.warning( "Cpl::Rte::Persistence::Record::Server::reportIncompatible - Incompatible schema" );
    m_localStatus = ServerStatus::eNO_STORAGE_WRONG_SCHEMA;
}

void Server::reportFileReadError() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportFileReadError") );
    m_logger.warning( "Cpl::Rte::Persistence::Record::Server::reportFileReadError - File Read/Open error" );
    m_localStatus = ServerStatus::eNO_STORAGE_MEDIA_ERR;
}

void Server::reportDataCorruptError() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportDataCorruptError") );
    m_logger.warning( "Cpl::Rte::Persistence::Record::Server::reportDataCorruptError - File Corruption error - defaulting one or more record(s)" );
    m_localStatus = ServerStatus::eRUNNING_CORRUPTED_INPUT;
}

void Server::reportFileWriteError() throw()
{
    const char* recname = m_writerPtr->getName();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportFileWriteError (rec=%s)", recname) );
    m_logger.warning( "Cpl::Rte::Persistence::Record::Server::reportFileWriteError - File Write error (rec=%s)", recname );

    // Delay reporting media error is I am the processing of 'opening'
    if ( m_localStatus == +ServerStatus::eOPENING )
    {
        m_localStatus = ServerStatus::eNO_STORAGE_MEDIA_ERR;
    }

    // Report the media error immediately if the DB is currently in the 'running' state
    else if ( m_localStatus == +ServerStatus::eRUNNING )
    {
        m_localStatus = ServerStatus::eNO_STORAGE_MEDIA_ERR;
        m_statusMp.write( m_localStatus );
    }
}

void Server::reportMinorUpgrade() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportMinorUpgrade") );
    m_logger.warning( "Cpl::Rte::Persistence::Record::Server::reportMinorUpgrade - one or more Records where automatically upgraded." );
    m_localStatus = ServerStatus::eRUNNING_MINOR_UPGRADE;
}



void Server::requestFileClose() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileClose") );
    m_chunkSAP.post( m_closeFileResponseMsg.getRequestMsg() );
}

void Server::requestFileOpen() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileOpen") );
    Cpl::Rte::Persistence::Chunk::Request::OpenFilePayload* payload = new(m_memPayload.m_byteMem)      Cpl::Rte::Persistence::Chunk::Request::OpenFilePayload( m_buffer, m_bufSize );
    Cpl::Rte::Persistence::Chunk::Response::OpenFileMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Cpl::Rte::Persistence::Chunk::Response::OpenFileMsg( *this, m_myMbox, m_chunkSAP, *payload );
    m_chunkSAP.post( rspmsg->getRequestMsg() );
}

void Server::requestFileClear() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileClear") );
    Cpl::Rte::Persistence::Chunk::Request::ClearFilePayload* payload = new(m_memPayload.m_byteMem)      Cpl::Rte::Persistence::Chunk::Request::ClearFilePayload( m_buffer, m_bufSize );
    Cpl::Rte::Persistence::Chunk::Response::ClearFileMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Cpl::Rte::Persistence::Chunk::Response::ClearFileMsg( *this, m_myMbox, m_chunkSAP, *payload );
    m_chunkSAP.post( rspmsg->getRequestMsg() );
}

void Server::requestFileRead() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileRead") );
    Cpl::Rte::Persistence::Chunk::Request::ReadPayload*  payload = new(m_memPayload.m_byteMem)      Cpl::Rte::Persistence::Chunk::Request::ReadPayload( m_buffer, m_bufSize, m_chunkHandle );
    Cpl::Rte::Persistence::Chunk::Response::ReadFileMsg* rspmsg  = new(m_memResponseMsg.m_byteMem)  Cpl::Rte::Persistence::Chunk::Response::ReadFileMsg( *this, m_myMbox, m_chunkSAP, *payload );
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    m_fileDataLen = 0;
}

void Server::requestFileWrite() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileWrite") );
    m_writerPtr = m_writeRequests.get();
    if ( !m_writerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Server::requestFileWrite - Protocol Error." );
    }

    const char* recName = m_writerPtr->getName();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileWrite - writing recordName=%s", recName) );
    plantRecName( recName );
    uint32_t dataLen = m_writerPtr->fillWriteBuffer( extractDataPointer(), maxAllowedDataSize() );

    Cpl::Rte::Persistence::Chunk::Request::WritePayload*  payload = new(m_memPayload.m_byteMem)      Cpl::Rte::Persistence::Chunk::Request::WritePayload( m_buffer, calcRecordLen( dataLen ), m_writerPtr->getChunkHandle() );
    Cpl::Rte::Persistence::Chunk::Response::WriteFileMsg* rspmsg  = new(m_memResponseMsg.m_byteMem)  Cpl::Rte::Persistence::Chunk::Response::WriteFileMsg( *this, m_myMbox, m_chunkSAP, *payload );
    m_chunkSAP.post( rspmsg->getRequestMsg() );
}


void Server::consumeNoWrite() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::consumeNoWrite") );
    m_writerPtr = m_writeRequests.get();
    if ( !m_writerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Server::consumeNoWrite - Protocol Error." );
    }
}


void Server::ackRead() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackRead") );
    Api_* recordPtr = findRecordApi( extractRecName(), extractRecNameLength() );

    // Failed record name look-up
    if ( !recordPtr )
    {
        // Log the failure
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackRead - Unknown Record Name=%.*s. Record will be purged", extractRecNameLength(), extractRecName()) );
        m_logger.warning( "Cpl::Rte::Persistence::Record::Server::ackRead - Unknown Record Name=%.*s. Record will be purged", extractRecNameLength(), extractRecName() );

        // Read the next record
        m_conversion = true;
        requestFileRead();
    }

    // Read raw chunk data into the Record's Point
    else
    {
        recordPtr->setChunkHandle( m_chunkHandle );
        if ( recordPtr->notifyRead( extractDataPointer(), calcDataLength() ) )
        {
            // Succesfull read --> start the read of the next record
            requestFileRead();
        }

        // Failed read!
        else
        {
            // Log the failure
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackRead - Record read failed (%.*s), i.e. an incompatible DB", extractRecNameLength(), extractRecName()) );
            m_logger.warning( "Cpl::Rte::Persistence::Record::Server::ackRead - Record read failed(%.*s), i.e. an incompatible DB", extractRecNameLength(), extractRecName() );

            // Send myself an 'bad-data' event to transition to the Non-Peristance State
            m_fileResult = Cpl::Rte::Persistence::Chunk::ServerResult::eWRONG_SCHEMA;
            generateEvent( HandlerFsm_evResponse );
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
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Server::ackWrite - Protocol Error." );
    }
}

void Server::defaultAllRecords() throw()
{
    // Default ALL registered records
    Api_* recordPtr = m_records.first();
    while ( recordPtr )
    {
        recordPtr->defaultContent();
        recordPtr = m_records.next( *recordPtr );
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

    Api_* recordPtr = m_records.first();
    while ( recordPtr )
    {
        recordPtr->notifyLoadCompleted();
        recordPtr = m_records.next( *recordPtr );
    }
}


void Server::nakOpenDone() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::nakOpenDone") );

    Api_* recordPtr = m_records.first();
    while ( recordPtr )
    {
        recordPtr->notifyLoadFailed();
        recordPtr = m_records.next( *recordPtr );
    }
}


void Server::ackFileStopped() throw()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackFileStopped") );

    // Something is BAD if the Record Handler has stopped and the Record Handler has NOT be requested to stop
    if ( !m_closeMsgPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::Server::ackFileStopped - Protocol error." );
    }

    // Return/complete my close request
    m_localStatus = ServerStatus::eCLOSED;
    m_statusMp.write( m_localStatus );
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
        generateEvent( HandlerFsm_evWrite );
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
        generateEvent( HandlerFsm_evVerified );
    }
    else
    {
        generateEvent( HandlerFsm_evIncompleteLoad );
    }
}



/////////////////////////////////
bool Server::isFileEof() throw()
{
    return m_fileResult == +Cpl::Rte::Persistence::Chunk::ServerResult::eEOF;
}

bool Server::isFileSuccess() throw()
{
    return m_fileResult == +Cpl::Rte::Persistence::Chunk::ServerResult::eSUCCESS;
}

bool Server::isFileBadData() throw()
{
    return m_fileResult == +Cpl::Rte::Persistence::Chunk::ServerResult::eCORRUPT_DATA;
}

bool Server::isFileError() throw()
{
    return m_fileResult == +Cpl::Rte::Persistence::Chunk::ServerResult::eERR_FILEIO || m_fileResult == +Cpl::Rte::Persistence::Chunk::ServerResult::eERR_NOT_A_MEDIA_FILE || m_fileResult == +Cpl::Rte::Persistence::Chunk::ServerResult::eERR_OPEN;
}

bool Server::isNotCompatible() throw()
{
    return m_fileResult == +Cpl::Rte::Persistence::Chunk::ServerResult::eWRONG_SCHEMA;
}


/////////////////////////////////
const char* Server::extractRecName( void )
{
    return (const char*) (m_buffer + Cpl::Rte::Persistence::eNLEN_SIZE);
}

uint16_t Server::extractRecNameLength( void )
{
    return *((uint16_t*) m_buffer);
}

uint32_t Server::calcDataLength( void )
{
    return m_fileDataLen - (Cpl::Rte::Persistence::eNLEN_SIZE + extractRecNameLength());
}

uint8_t* Server::extractDataPointer( void )
{
    return m_buffer + Cpl::Rte::Persistence::eNLEN_SIZE + extractRecNameLength();
}

uint32_t Server::maxAllowedDataSize( void ) {
    return m_bufSize - (Cpl::Rte::Persistence::eNLEN_SIZE + extractRecNameLength());
}

void Server::plantRecName( const char* recName )
{
    uint16_t nameLen = (uint16_t) strlen( recName );
    *((uint16_t*) m_buffer) = nameLen;
    memcpy( m_buffer + Cpl::Rte::Persistence::eNLEN_SIZE, recName, nameLen );
}

uint32_t Server::calcRecordLen( uint32_t datalen )
{
    return datalen + Cpl::Rte::Persistence::eNLEN_SIZE + extractRecNameLength();
}


Api_* Server::findRecordApi( const char* recName, uint16_t nameLen )
{
    Cpl::Container::KeyStringBuffer recordNameKey( recName, nameLen );
    return m_records.find( recordNameKey );
}
