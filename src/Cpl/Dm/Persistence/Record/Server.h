#ifndef Cpl_Rte_Persistence_Record_Server_h_
#define Cpl_Rte_Persistence_Record_Server_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Server Project.  The Colony.Server Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Dm/Persistence/Record/ServerApi.h"
#include "Cpl/Dm/Persistence/Chunk/Request.h"
#include "Cpl/Dm/Persistence/Record/HandlerApi_.h"
#include "Cpl/Dm/Persistence/Record/HandlerFsmEventQueue_.h"
#include "Cpl/Dm/Persistence/Record/MpServerStatus.h"
#include "Cpl/Dm/Persistence/Record/ApiWriter_.h"
#include "Cpl/Dm/Persistence/Record/Api_.h"
#include "Cpl/Dm/Mp/String.h"
#include "Cpl/Dm/SubscriberApi.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Container/SList.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Memory/Aligned.h"
#include <stdint.h>


/// Namespace(s)
namespace Cpl {
namespace Dm {
namespace Persistence {
namespace Record {


/** This class implements the core functionality of the record layer.  There
    is single handler than manages all of the records in a single DB.  If there
    are multiple DBs - then the domain application need to instance multiple
    instance of this class.
 */
class Server : public ServerApi,
    public Cpl::Dm::Persistence::Chunk::Response,
    public HandlerApi_,
    public HandlerFsmEventQueue_,
    public Subscriber<Cpl::Dm::Mp::String>

{
private:
    /// Remember if at least one set performed a 'conversion'
    bool                                        m_conversion;

    /// Number of Registered records
    unsigned                                    m_totalRecordCount;

    /// Number of Records that have NOT been successfully initialized
    unsigned                                    m_recordCount;

    /// Number of started/opened Records
    unsigned                                    m_openCount;

    /// Number of stopped/closed Records
    unsigned                                    m_closeCount;

    /// Cache my Open Request message
    OpenMsg*                                    m_openMsgPtr;

    /// Cache my Close Request message
    CloseMsg*                                   m_closeMsgPtr;

    /// Current Health MP Reference
    MpServerStatus&                             m_statusMp;

    /// Temporary/intermediate status
    ServerStatus                                m_localStatus;

    /// 'Default Action request' MP Reference
    Cpl::Dm::Mp::String&                       m_defaultRequestMp;

    /// Raw buffer used for all Read/Write operations
    uint8_t*                                    m_buffer;

    /// Size, in bytes, of m_buffer
    uint32_t                                    m_bufSize;

    /// SAP of the Chunk Layer/server
    Dm::Persistence::Chunk::Request::SAP&      m_chunkSAP;

    /// My ITC mailbox
    Cpl::Dm::MailboxServer&                    m_myMbox;

    /// The result code for the last Chunk action
    Dm::Persistence::Chunk::ServerResult       m_fileResult;

    /// Working Chunk handle
    Dm::Persistence::Chunk::Handle             m_chunkHandle;

    /// Working len of the data being read/written
    uint32_t                                    m_fileDataLen;

    /// Current Record Writer
    ApiWriter_*                                 m_writerPtr;

    /// Queued/pending Write requests
    Cpl::Container::SList<ApiWriter_>           m_writeRequests;

    /// Map of expected/supported Records
    Cpl::Container::Map<Api_>&                  m_records;

    /// Close DB msg  (I have a separate CloseFileMsg since it can be invoked asynchronously in relation to all other Chunk ITC messages)
    Dm::Persistence::Chunk::Response::CloseFileMsg    m_closeFileResponseMsg;

    /// Close File payload
    Dm::Persistence::Chunk::Request::CloseFilePayload m_closeFilePayload;


    /// Memory for Chunk Layer ITC requests (use the 'largest' payload/msg)
    Cpl::Memory::AlignedClass<Dm::Persistence::Chunk::Request::WritePayload>   m_memPayload;

    /// Memory for Chunk Layer ITC requests
    Cpl::Memory::AlignedClass<Dm::Persistence::Chunk::Response::WriteFileMsg>  m_memResponseMsg;


public:
    /// Constructor
    Server( uint8_t*                                    recordLayerBuffer,
            uint32_t                                    bufferSize,
            Cpl::Dm::Persistence::Chunk::Request::SAP&  chunkSAP,
            Cpl::Dm::MailboxServer&                     recordLayerMbox,
            Cpl::Container::Map<Api_>&                  recordList,
            MpServerStatus&                             serverStatusMp,
            Cpl::Dm::Mp::String&                        defaultRequestMp
    );

public:
    /// See Cpl::Dm::Subscriber<String>
    void modelPointChanged( Cpl::Dm::Mp::String& modelPointThatChanged ) noexcept;

public:
    /// See Cpl::Itc::Open
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /// See Cpl::Itc::Close
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


public:
    /// See Cpl::Dm::Persistence::Record::HandlerLocal
    void notifyRecordWaiting( void );

    /// See Cpl::Dm::Persistence::Record::HandlerLocal
    void notifyRecordInitialized( void );

    /// See Cpl::Dm::Persistence::Record::HandlerLocal
    void notifyRecordConverted( void );

    /// See Cpl::Dm::Persistence::Record::HandlerLocal
    void notifyRecordStopped( void );

    /// See Cpl::Dm::Persistence::Record::HandlerLocal
    void notifyRecordStarted( void );

    /// See Cpl::Dm::Persistence::Record::HandlerLocal
    void write( ApiWriter_& recordToWrite );


protected:
    /// See Dm::Persistence::Chunk::Response
    void response( OpenFileMsg& msg );

    /// See Dm::Persistence::Chunk::Response
    void response( CloseFileMsg& msg );

    /// See Dm::Persistence::Chunk::Response
    void response( ClearFileMsg& msg );

    /// See Dm::Persistence::Chunk::Response
    void response( ReadFileMsg& msg );

    /// See Dm::Persistence::Chunk::Response
    void response( WriteFileMsg& msg );


protected:
    /// See Dm::Persistence::Record::HandlerFsmContext_
    void ackFileStopped() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void ackOpenDone() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void ackRead() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void ackWrite() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void clearWriteQue() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void consumeNoWrite() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void defaultAllRecords() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void inspectWriteQue() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void nakOpenDone() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void nakWrite() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void queWriteRequest() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void reportDataCorruptError() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void reportFileReadError() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void reportFileWriteError() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void reportIncompatible() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void reportMinorUpgrade() noexcept;


    /// See Dm::Persistence::Record::HandlerFsmContext_
    void requestFileClear() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void requestFileClose() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void requestFileOpen() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void requestFileRead() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void requestFileWrite() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    void verifyOpen() noexcept;


protected:
    /// See Dm::Persistence::Record::HandlerFsmContext_
    bool isFileBadData() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    bool isFileEof() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    bool isFileError() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    bool isFileSuccess() noexcept;

    /// See Dm::Persistence::Record::HandlerFsmContext_
    bool isNotCompatible() noexcept;


protected:
    /// Helper
    const char* extractRecName( void );

    /// Helper
    uint16_t    extractRecNameLength( void );

    /// Helper
    uint8_t*    extractDataPointer( void );

    /// Helper
    void        plantRecName( const char* recName );

    /// Helper
    uint32_t    calcDataLength( void );

    /// Helper
    uint32_t    calcRecordLen( uint32_t datalen );

    /// Helper
    uint32_t    maxAllowedDataSize( void );


protected:
    /// Helper
    Api_* findRecordApi( const char* recName, uint16_t nameLen );




};

};      // end namespaces
};
};
};
#endif  // end header latch
