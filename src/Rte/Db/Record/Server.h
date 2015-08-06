#ifndef Rte_Db_Record_Server_h_
#define Rte_Db_Record_Server_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Server Project.  The Colony.Server Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014, 2015  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Rte/Db/Chunk/Request.h"
#include "Rte/Db/Record/ApiLocal.h"
#include "Rte/Db/Record/Handler.h"
#include "Rte/Db/Record/HandlerFsmEventQueue_.h"
#include "Rte/Db/Record/HealthRequest.h"
#include "Rte/Db/Record/DefaultRequest.h"
#include "Cpl/Container/DList.h"
#include "Cpl/Container/SList.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Memory/Aligned.h"
#include "Cpl/Log/Loggers.h"
#include <stdint.h>


/// Namespace(s)
namespace Rte { namespace Db { namespace Record { 


/** This class implements the core functionality of the record layer.  There
    is single handler than manages all of the records in a single DB.  If there
    are multiple DBs - then the domain application need to instance multiple 
    instance of this class. 
 */
class Server: public Rte::Db::Chunk::Response,
              public Handler,
              public HandlerLocal,
              public HandlerFsmEventQueue_,
              public HealthRequest,
              public DefaultRequest

{
private:
    /// List of Clients monitor my health
    Cpl::Container::DList<RegisterMsg>      m_pendingHealth;

    /// SAP for my Viewer ITC interface
    HealthRequest::SAP                      m_healthSAP;

    /// Remember if at least one set performed a 'conversion'
    bool                                    m_conversion;

    /// Number of Records that have NOT been succesfully initialized
    unsigned                                m_recordCount;

    /// Number of started/opened Records
    unsigned                                m_openCount;

    /// Number of stopped/closed Records
    unsigned                                m_closeCount;
                                        
    /// Cache my Open Request message
    OpenMsg*                                m_openMsgPtr;

    /// Cache my Close Request message
    CloseMsg*                               m_closeMsgPtr;

    /// Current Health
    HealthRequest::Status_T                 m_status;

    /// Temporary/working Health
    HealthRequest::Status_T                 m_tempStatus;

    /// Raw buffer used for all Read/Write operaitons
    uint8_t*                                m_buffer;

    /// Size, in bytes, of m_buffer
    uint32_t                                m_bufSize;

    /// SAP of the Chunk Layer/server
    Rte::Db::Chunk::Request::SAP&           m_chunkSAP;

    /// My ITC mailbox
    Cpl::Itc::PostApi&                      m_myMbox;

    /// The result code for the last Chunk action
    Rte::Db::Chunk::Request::Result_T       m_dbResult;

    /// Working Chunk handle
    Rte::Db::Chunk::Handle                  m_chunkHandle;

    /// Working len of the data being read/written
    uint32_t                                m_dbDataLen;

    /// Current Record Writer
    ApiLocalWriter*                         m_writerPtr;

    /// Logger (for unexpected events)
    Cpl::Log::Api&                          m_logger;

    /// Queued/pending Write requests
    Cpl::Container::SList<ApiLocalWriter>   m_writeRequests;

    /// Map of expected/supported Records
    Cpl::Container::Map<ApiLocal>&          m_records;

    /// Close DB payload
    Rte::Db::Chunk::Request::CloseDbPayload m_closeDbPayload;

    /// Close DB msg  (I have a seperate closeDbMsg since it can be invoked asynchronously in relation to all other Chunk ITC messages)
    Rte::Db::Chunk::Response::CloseDbMsg    m_closeDbResponseMsg;

    /// Memmory for Chunk Layer ITC requests (use the 'largest' payload/msg)
    Cpl::Memory::AlignedClass<Rte::Db::Chunk::Request::WritePayload>   m_memPayload;

    /// Memmory for Chunk Layer ITC requests
    Cpl::Memory::AlignedClass<Rte::Db::Chunk::Response::WriteMsg>      m_memResponseMsg;


public:
    /// Constructor
    Server( uint8_t*                       recordLayerBuffer,
            uint32_t                       bufferSize,
            Rte::Db::Chunk::Request::SAP&  chunkSAP,
            Cpl::Itc::PostApi&             recordLayerMbox,
            Cpl::Container::Map<ApiLocal>& recordList,
            Cpl::Log::Api&                 eventLogger  = Cpl::Log::Loggers::application()
          );

public:
    /// See Rte::Db::Record::Handler
    HealthRequest::SAP&  getHealthSAP(void);

    /// See Rte::Db::Record::Handler
    void defaultAllRecordsContent() throw();

    /// See Rte::Db::Record::HealthRequest
    void request( RegisterMsg& msg );

    /// See Rte::Db::Record::HealthRequest
    void request( CancelMsg& msg );

    /// See Rte::Db::Record::DefaultRequest
    void request( DefaultMsg& msg );

    /// See Cpl::Itc::Open
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /// See Cpl::Itc::Close
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


public:
    /// See Rte::Db::Record::HandlerLocal
    void notifyRecordWaiting( void );

    /// See Rte::Db::Record::HandlerLocal
    void notifyRecordInitialized( void );

    /// See Rte::Db::Record::HandlerLocal
    void notifyRecordConverted( void );

    /// See Rte::Db::Record::HandlerLocal
    void notifyRecordStopped( void );

    /// See Rte::Db::Record::HandlerLocal
    void notifyRecordStarted( void );

    /// See Rte::Db::Record::HandlerLocal
    void write( ApiLocalWriter& recordToWrite );


protected:
    /// See Rte::Db::Chunk::Response
    void response( OpenDbMsg& msg );

    /// Response: Close Database
    void response( CloseDbMsg& msg );
    
    /// Response: Clear Database
    void response( ClearDbMsg& msg );
    
    /// Response: Read action
    void response( ReadMsg& msg );
    
    /// Response: Write action
    void response( WriteMsg& msg );


protected:
    /// See Rte::Db::Record::HandlerFsmContext_
    void ackDbStopped() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void ackOpenDone() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void ackRead() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void ackWrite() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void clearWriteQue() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void consumeNoWrite() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void inspectWriteQue() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void nakOpenDone() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void nakWrite() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void queWriteRequest() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void reportDataCorruptError() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void reportFileReadError() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void reportFileWriteError() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void reportIncompatible() throw();
    
    /// See Rte::Db::Record::HandlerFsmContext_
    void requestDbClear() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void requestDbClose() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void requestDbOpen() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void requestDbRead() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void requestDbWrite() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    void verifyOpen() throw();


protected:
    /// See Rte::Db::Record::HandlerFsmContext_
    bool isDbBadData() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    bool isDbEof() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    bool isDbError() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    bool isDbSuccess() throw();

    /// See Rte::Db::Record::HandlerFsmContext_
    bool isNotCompatible() throw();


protected:
    /// Helper
    const char* extractRecName(void);

    /// Helper
    uint16_t    extractRecNameLength(void);

    /// Helper
    uint8_t*    extractDataPointer(void);

    /// Helper
    void        plantRecName( const char* recName );

    /// Helper
    uint32_t    calcDataLength(void);

    /// Helper
    uint32_t    calcRecordLen( uint32_t datalen );

    /// Helper
    uint32_t    maxAllowedDataSize(void);


protected:
    /// Helper
    void setNewHealthStatus( HealthRequest::Status_T newstatus );

    /// Helper
    ApiLocal* findRecordApi( const char* recName, uint16_t nameLen );




};

};      // end namespaces
};
};
#endif  // end header latch
