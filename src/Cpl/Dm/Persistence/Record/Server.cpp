/*-----------------------------------------------------------------------------
* This file is part of the Shift-Right Open Repository.  The Repository is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the root directory or on the Internet at
* http://www.shift-right.com/openrepo/license.htm
*
* Copyright (c) 2010-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Server.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/Dm/Persistence/Framing.h"
#include "Cpl/Text/FString.h"
#include <string.h>
#include <new>

using namespace Cpl::Dm::Persistence::Record;

#define SECT_   "Cpl::Dm::Persistence::Record"

/////////////////////////////////
Server::Server( uint8_t*                                    recordLayerBuffer,
                uint32_t                                    bufferSize,
                Cpl::Dm::Persistence::Chunk::Request::SAP&  chunkSAP,
                Cpl::Dm::MailboxServer&                     recordLayerMbox,
                Cpl::Container::Map<Api_>&                  recordList,
                MpServerStatus&                             serverStatusMp,
                Cpl::Dm::Mp::String&                        defaultRequestMp )
    :ServerApi( recordLayerMbox )
    , Subscriber<Cpl::Dm::Mp::String>( recordLayerMbox )
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
    , m_fileResult( Cpl::Dm::Persistence::Chunk::ServerResult::eSUCCESS )
    , m_fileDataLen( 0 )
    , m_writerPtr( 0 )
    , m_records( recordList )
    , m_closeFileResponseMsg( *this, recordLayerMbox, chunkSAP, m_closeFilePayload )
{
    // Initialize my FSM
    initialize();
}

///////////////////////////////////////
void Server::modelPointChanged( Cpl::Dm::Mp::String& modelPointThatChanged ) noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::modelPointChanged") );

    Cpl::Text::FString<OPTION_CPL_DM_PERSISTENCE_RECORD_MAX_RECORD_NAME_LEN> name;
    if ( Cpl::Dm::ModelPoint::IS_VALID( modelPointThatChanged.read( name ) ) )
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
            }
        }

        // Clear the request (but do in any that I don't get a change notification for the clear action)
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
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Server::start().  Protocol error.  The FSM is NOT in the idle state (currentState=%s)", getNameByState( getInnermostActiveState() ) );
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

    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Server::request::OpenMsg - record count=%d", m_totalRecordCount ) );

    // Do NOTHING if I don't have registered records
    if ( m_openCount == 0 )
    {
        m_openMsgPtr  = 0;
        msg.returnToSender();
        return;
    }

    // Start all registered records (MUST be done AFTER I have obtained my total record count) 
    recordPtr = m_records.first();
    while ( recordPtr )
    {
        recordPtr->start( *this );
        recordPtr = m_records.next( *recordPtr );
    }



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
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Server::request::CloseMsg - Protocol error. The FSM is IN the idle state." );
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
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Server::notifyRecordStopped - Protocol error." );
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
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Server::notifyRecordStarted - Protocol error." );
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
            // Set error result to the open request if in 'Non-Persistence' mode
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
void Server::reportIncompatible() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportIncompatible") );
    m_localStatus = ServerStatus::eNO_STORAGE_WRONG_SCHEMA;
}

void Server::reportFileReadError() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportFileReadError") );
    m_localStatus = ServerStatus::eNO_STORAGE_MEDIA_ERR;
}

void Server::reportDataCorruptError() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportDataCorruptError") );
    m_localStatus = ServerStatus::eRUNNING_CORRUPTED_INPUT;
}

void Server::reportFileWriteError() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportFileWriteError (rec=%s)", m_writerPtr->getName() ) );

    // Delay reporting media error if I am the processing of 'opening'
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

void Server::reportMinorUpgrade() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::reportMinorUpgrade") );
    m_localStatus = ServerStatus::eRUNNING_MINOR_UPGRADE;
}



void Server::requestFileClose() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileClose") );
    m_chunkSAP.post( m_closeFileResponseMsg.getRequestMsg() );
}

void Server::requestFileOpen() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileOpen") );
    Cpl::Dm::Persistence::Chunk::Request::OpenFilePayload* payload = new(m_memPayload.m_byteMem)      Cpl::Dm::Persistence::Chunk::Request::OpenFilePayload( m_buffer, m_bufSize );
    Cpl::Dm::Persistence::Chunk::Response::OpenFileMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Cpl::Dm::Persistence::Chunk::Response::OpenFileMsg( *this, m_myMbox, m_chunkSAP, *payload );
    m_chunkSAP.post( rspmsg->getRequestMsg() );
}

void Server::requestFileClear() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileClear") );
    Cpl::Dm::Persistence::Chunk::Request::ClearFilePayload* payload = new(m_memPayload.m_byteMem)      Cpl::Dm::Persistence::Chunk::Request::ClearFilePayload( m_buffer, m_bufSize );
    Cpl::Dm::Persistence::Chunk::Response::ClearFileMsg*    rspmsg  = new(m_memResponseMsg.m_byteMem)  Cpl::Dm::Persistence::Chunk::Response::ClearFileMsg( *this, m_myMbox, m_chunkSAP, *payload );
    m_chunkSAP.post( rspmsg->getRequestMsg() );
}

void Server::requestFileRead() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileRead") );
    Cpl::Dm::Persistence::Chunk::Request::ReadPayload*  payload = new(m_memPayload.m_byteMem)      Cpl::Dm::Persistence::Chunk::Request::ReadPayload( m_buffer, m_bufSize, m_chunkHandle );
    Cpl::Dm::Persistence::Chunk::Response::ReadFileMsg* rspmsg  = new(m_memResponseMsg.m_byteMem)  Cpl::Dm::Persistence::Chunk::Response::ReadFileMsg( *this, m_myMbox, m_chunkSAP, *payload );
    m_chunkSAP.post( rspmsg->getRequestMsg() );
    m_fileDataLen = 0;
}

void Server::requestFileWrite() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileWrite") );
    m_writerPtr = m_writeRequests.get();
    if ( !m_writerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Server::requestFileWrite - Protocol Error." );
    }

    const char* recName = m_writerPtr->getName();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::requestFileWrite - writing recordName=%s", recName) );
    plantRecName( recName );
    uint32_t dataLen = m_writerPtr->fillWriteBuffer( extractDataPointer(), maxAllowedDataSize() );

    Cpl::Dm::Persistence::Chunk::Request::WritePayload*  payload = new(m_memPayload.m_byteMem)      Cpl::Dm::Persistence::Chunk::Request::WritePayload( m_buffer, calcRecordLen( dataLen ), m_writerPtr->getChunkHandle() );
    Cpl::Dm::Persistence::Chunk::Response::WriteFileMsg* rspmsg  = new(m_memResponseMsg.m_byteMem)  Cpl::Dm::Persistence::Chunk::Response::WriteFileMsg( *this, m_myMbox, m_chunkSAP, *payload );
    m_chunkSAP.post( rspmsg->getRequestMsg() );
}


void Server::consumeNoWrite() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::consumeNoWrite") );
    m_writerPtr = m_writeRequests.get();
    if ( !m_writerPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Server::consumeNoWrite - Protocol Error." );
    }
}


void Server::ackRead() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackRead") );
    Api_* recordPtr = findRecordApi( extractRecName(), extractRecNameLength() );

    // Failed record name look-up
    if ( !recordPtr )
    {
        // Log the failure
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackRead - Unknown Record Name=%.*s. Record will be purged", extractRecNameLength(), extractRecName()) );

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

            // Send myself an 'bad-data' event to transition to the Non-Persistence State
            m_fileResult = Cpl::Dm::Persistence::Chunk::ServerResult::eWRONG_SCHEMA;
            generateEvent( HandlerFsm_evResponse );
        }
    }
}

void Server::ackWrite() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackWrite") );
    if ( m_writerPtr )
    {
        m_writerPtr->notifyWriteDone();
        m_writerPtr = 0;
    }
    else
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Server::ackWrite - Protocol Error." );
    }
}

void Server::defaultAllRecords() noexcept
{
    // Default ALL registered records
    Api_* recordPtr = m_records.first();
    while ( recordPtr )
    {
        recordPtr->defaultContent();
        recordPtr = m_records.next( *recordPtr );
    }
}

void Server::nakWrite() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::nakWrite") );
    m_writerPtr = 0;
}


void Server::ackOpenDone() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackOpenDone") );

    Api_* recordPtr = m_records.first();
    while ( recordPtr )
    {
        recordPtr->notifyLoadCompleted();
        recordPtr = m_records.next( *recordPtr );
    }
}


void Server::nakOpenDone() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::nakOpenDone") );

    Api_* recordPtr = m_records.first();
    while ( recordPtr )
    {
        recordPtr->notifyLoadFailed();
        recordPtr = m_records.next( *recordPtr );
    }
}


void Server::ackFileStopped() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::ackFileStopped") );

    // Something is BAD if the Record Handler has stopped and the Record Handler has NOT be requested to stop
    if ( !m_closeMsgPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Persistence::Record::Server::ackFileStopped - Protocol error." );
    }

    // Return/complete my close request
    m_localStatus = ServerStatus::eCLOSED;
    m_statusMp.write( m_localStatus );
    m_closeMsgPtr->returnToSender();
    m_closeMsgPtr = 0;
}

void Server::queWriteRequest() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::queWriteRequest") );
    // Nothing needed!
}

void Server::inspectWriteQue() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::inspectWriteQue") );

    // Generate a write event if there is NOT a write an progress AND there is at least one queued write request
    if ( m_writerPtr == 0 && m_writeRequests.first() )
    {
        generateEvent( HandlerFsm_evWrite );
    }
}

void Server::clearWriteQue() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::clearWriteQue") );
    m_writeRequests.clearTheList();
    m_writerPtr = 0;
}

void Server::verifyOpen() noexcept
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
bool Server::isFileEof() noexcept
{
    return m_fileResult == +Cpl::Dm::Persistence::Chunk::ServerResult::eEOF;
}

bool Server::isFileSuccess() noexcept
{
    return m_fileResult == +Cpl::Dm::Persistence::Chunk::ServerResult::eSUCCESS;
}

bool Server::isFileBadData() noexcept
{
    return m_fileResult == +Cpl::Dm::Persistence::Chunk::ServerResult::eCORRUPT_DATA;
}

bool Server::isFileError() noexcept
{
    return m_fileResult == +Cpl::Dm::Persistence::Chunk::ServerResult::eERR_FILEIO || m_fileResult == +Cpl::Dm::Persistence::Chunk::ServerResult::eERR_NOT_A_MEDIA_FILE || m_fileResult == +Cpl::Dm::Persistence::Chunk::ServerResult::eERR_OPEN;
}

bool Server::isNotCompatible() noexcept
{
    return m_fileResult == +Cpl::Dm::Persistence::Chunk::ServerResult::eWRONG_SCHEMA;
}


/////////////////////////////////
const char* Server::extractRecName( void )
{
    return (const char*) (m_buffer + Cpl::Dm::Persistence::eNLEN_SIZE);
}

uint16_t Server::extractRecNameLength( void )
{
    return *((uint16_t*) m_buffer);
}

uint32_t Server::calcDataLength( void )
{
    return m_fileDataLen - (Cpl::Dm::Persistence::eNLEN_SIZE + extractRecNameLength());
}

uint8_t* Server::extractDataPointer( void )
{
    return m_buffer + Cpl::Dm::Persistence::eNLEN_SIZE + extractRecNameLength();
}

uint32_t Server::maxAllowedDataSize( void ) {
    return m_bufSize - (Cpl::Dm::Persistence::eNLEN_SIZE + extractRecNameLength());
}

void Server::plantRecName( const char* recName )
{
    uint16_t nameLen = (uint16_t) strlen( recName );
    *((uint16_t*) m_buffer) = nameLen;
    memcpy( m_buffer + Cpl::Dm::Persistence::eNLEN_SIZE, recName, nameLen );
}

uint32_t Server::calcRecordLen( uint32_t datalen )
{
    return datalen + Cpl::Dm::Persistence::eNLEN_SIZE + extractRecNameLength();
}


Api_* Server::findRecordApi( const char* recName, uint16_t nameLen )
{
    Cpl::Container::KeyStringBuffer recordNameKey( recName, nameLen );
    return m_records.find( recordNameKey );
}
