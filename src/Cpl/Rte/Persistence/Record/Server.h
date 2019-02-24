#ifndef Cpl_Rte_Persistence_Record_Server_h_
#define Cpl_Rte_Persistence_Record_Server_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Server Project.  The Colony.Server Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Rte/Persistence/Record/ServerApi.h"
#include "Cpl/Rte/Persistence/Chunk/Request.h"
#include "Cpl/Rte/Persistence/Record/HandlerApi_.h"
#include "Cpl/Rte/Persistence/Record/HandlerFsmEventQueue_.h"
#include "Cpl/Rte/Persistence/Record/MpServerStatus.h"
#include "Cpl/Rte/Persistence/Record/ApiWriter_.h"
#include "Cpl/Rte/Persistence/Record/Api_.h"
#include "Cpl/Rte/Mp/String.h"
#include "Cpl/Rte/SubscriberApi.h"
#include "Cpl/Container/SList.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Memory/Aligned.h"
#include <stdint.h>


/// Namespace(s)
namespace Cpl {
namespace Rte {
namespace Persistence {
namespace Record {


/** This class implements the core functionality of the record layer.  There
    is single handler than manages all of the records in a single DB.  If there
    are multiple DBs - then the domain application need to instance multiple
    instance of this class.
 */
class Server : public ServerApi,
    public Cpl::Rte::Persistence::Chunk::Response,
    public HandlerApi_,
    public HandlerFsmEventQueue_,
    public Subscriber<Cpl::Rte::Mp::String>

{
private:
    /// Remember if at least one set performed a 'conversion'
    bool                                        m_conversion;

    /// Number of Registered records
    unsigned                                    m_totalRecordCount;

    /// Number of Records that have NOT been succesfully initialized
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

    /// Temporary/intermeidate status
    ServerStatus                                m_localStatus;

    /// 'Default Action request' MP Reference
    Cpl::Rte::Mp::String&                       m_defaultRequestMp;

    /// Raw buffer used for all Read/Write operaitons
    uint8_t*                                    m_buffer;

    /// Size, in bytes, of m_buffer
    uint32_t                                    m_bufSize;

    /// SAP of the Chunk Layer/server
    Rte::Persistence::Chunk::Request::SAP&      m_chunkSAP;

    /// My ITC mailbox
    Cpl::Rte::MailboxServer&                    m_myMbox;

    /// The result code for the last Chunk action
    Rte::Persistence::Chunk::ServerResult       m_fileResult;

    /// Working Chunk handle
    Rte::Persistence::Chunk::Handle             m_chunkHandle;

    /// Working len of the data being read/written
    uint32_t                                    m_fileDataLen;

    /// Current Record Writer
    ApiWriter_*                                 m_writerPtr;

    /// Logger (for unexpected events)
    Cpl::Log::Api&                              m_logger;

    /// Queued/pending Write requests
    Cpl::Container::SList<ApiWriter_>           m_writeRequests;

    /// Map of expected/supported Records
    Cpl::Container::Map<Api_>&                  m_records;

    /// Close DB msg  (I have a seperate CloseFileMsg since it can be invoked asynchronously in relation to all other Chunk ITC messages)
    Rte::Persistence::Chunk::Response::CloseFileMsg    m_closeFileResponseMsg;

    /// Close File payload
    Rte::Persistence::Chunk::Request::CloseFilePayload m_closeFilePayload;


    /// Memmory for Chunk Layer ITC requests (use the 'largest' payload/msg)
    Cpl::Memory::AlignedClass<Rte::Persistence::Chunk::Request::WritePayload>   m_memPayload;

    /// Memmory for Chunk Layer ITC requests
    Cpl::Memory::AlignedClass<Rte::Persistence::Chunk::Response::WriteFileMsg>  m_memResponseMsg;


public:
    /// Constructor
    Server( uint8_t*                                    recordLayerBuffer,
            uint32_t                                    bufferSize,
            Cpl::Rte::Persistence::Chunk::Request::SAP& chunkSAP,
            Cpl::Rte::MailboxServer&                    recordLayerMbox,
            Cpl::Container::Map<Api_>&                  recordList,
            MpServerStatus&                             serverStatusMp,
            Cpl::Rte::Mp::String&                       defaultRequestMp,
            Cpl::Log::Api&                              eventLogger  = Cpl::Log::Loggers::application()
    );

public:
    /// See Cpl::Rte::Subscriber<String>
    void modelPointChanged( Cpl::Rte::Mp::String& modelPointThatChanged ) throw();

public:
    /// See Cpl::Itc::Open
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /// See Cpl::Itc::Close
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


public:
    /// See Cpl::Rte::Persistence::Record::HandlerLocal
    void notifyRecordWaiting( void );

    /// See Cpl::Rte::Persistence::Record::HandlerLocal
    void notifyRecordInitialized( void );

    /// See Cpl::Rte::Persistence::Record::HandlerLocal
    void notifyRecordConverted( void );

    /// See Cpl::Rte::Persistence::Record::HandlerLocal
    void notifyRecordStopped( void );

    /// See Cpl::Rte::Persistence::Record::HandlerLocal
    void notifyRecordStarted( void );

    /// See Cpl::Rte::Persistence::Record::HandlerLocal
    void write( ApiWriter_& recordToWrite );


protected:
    /// See Rte::Persistence::Chunk::Response
    void response( OpenFileMsg& msg );

    /// See Rte::Persistence::Chunk::Response
    void response( CloseFileMsg& msg );

    /// See Rte::Persistence::Chunk::Response
    void response( ClearFileMsg& msg );

    /// See Rte::Persistence::Chunk::Response
    void response( ReadFileMsg& msg );

    /// See Rte::Persistence::Chunk::Response
    void response( WriteFileMsg& msg );


protected:
    /// See Rte::Persistence::Record::HandlerFsmContext_
    void ackFileStopped() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void ackOpenDone() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void ackRead() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void ackWrite() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void clearWriteQue() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void consumeNoWrite() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void defaultAllRecords() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void inspectWriteQue() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void nakOpenDone() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void nakWrite() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void queWriteRequest() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void reportDataCorruptError() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void reportFileReadError() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void reportFileWriteError() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void reportIncompatible() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void reportMinorUpgrade() throw();


    /// See Rte::Persistence::Record::HandlerFsmContext_
    void requestFileClear() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void requestFileClose() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void requestFileOpen() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void requestFileRead() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void requestFileWrite() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    void verifyOpen() throw();


protected:
    /// See Rte::Persistence::Record::HandlerFsmContext_
    bool isFileBadData() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    bool isFileEof() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    bool isFileError() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    bool isFileSuccess() throw();

    /// See Rte::Persistence::Record::HandlerFsmContext_
    bool isNotCompatible() throw();


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
