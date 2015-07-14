#ifndef Rte_Db_Record_Core_h_
#define Rte_Db_Record_Core_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
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
#include "Rte/Db/Record/Handler.h"
#include "Rte/Db/Record/Client.h"
#include "Rte/Db/Record/ErrorClient.h"
#include "Rte/Db/Record/Fsm_.h"
#include "Cpl/Container/SList.h"
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
class Core: public Rte::Db::Chunk::Response,
            public Handler,
            public Fsm
{
private:
    /// Raw buffer used for all Read/Write operaitons
    uint8_t*                                m_buffer;

    /// Size, in bytes, of m_buffer
    uint32_t                                m_bufSize;

    /// Set Layer handler (i.e. my client/consumer)
    Client&                                 m_setLayer;

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

    /// Current Record
    Api*                                    m_recordPtr;

    /// Event Queue (to ensure run-to-completion semantics for the FSM)
    FSM_EVENT_T                             m_queuedEvent;

    /// Logger (for unexpected events)
    Cpl::Log::Api&                          m_logger;

    /// Interface (if any) to report errors
    ErrorClient*                            m_errHandlerPtr;

    /// Queued/pending Write requests
    Cpl::Container::SList<Api>              m_writeRequests;

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
    Core( uint8_t*                      recordLayerBuffer,
          uint32_t                      bufferSize,
          Client&                       setLayerHandler,
          Rte::Db::Chunk::Request::SAP& chunkSAP,
          Cpl::Itc::PostApi&            recordLayerMbox,
          Cpl::Log::Api&                eventLogger  = Cpl::Log::Loggers::application(),
          ErrorClient*                  errorHandler = 0   // Optional
        );


public: 
    /// See Rte::Db::Record::Handler
    void start( void );

    /// See Rte::Db::Record::Handler
    void stop( void );

    /// See Rte::Db::Record::Handler
    void write( Api& recordToWrite );


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
    /// See Rte::Db::Record::FsmContext_
    void ackDbStopped() throw();

    /// See Rte::Db::Record::FsmContext_
    void ackOpenDone() throw();

    /// See Rte::Db::Record::FsmContext_
    void ackRead() throw();

    /// See Rte::Db::Record::FsmContext_
    void ackWrite() throw();

    /// See Rte::Db::Record::FsmContext_
    void clearWriteQue() throw();

    /// See Rte::Db::Record::FsmContext_
    void consumeNoWrite() throw();

    /// See Rte::Db::Record::FsmContext_
    void inspectWriteQue() throw();

    /// See Rte::Db::Record::FsmContext_
    void nakOpenDone() throw();

    /// See Rte::Db::Record::FsmContext_
    void nakWrite() throw();

    /// See Rte::Db::Record::FsmContext_
    void queWriteRequest() throw();

    /// See Rte::Db::Record::FsmContext_
    void reportDataCorruptError() throw();

    /// See Rte::Db::Record::FsmContext_
    void reportFileReadError() throw();

    /// See Rte::Db::Record::FsmContext_
    void reportFileWriteError() throw();

    /// See Rte::Db::Record::FsmContext_
    void reportIncompatible() throw();
    
    /// See Rte::Db::Record::FsmContext_
    void requestDbClear() throw();

    /// See Rte::Db::Record::FsmContext_
    void requestDbClose() throw();

    /// See Rte::Db::Record::FsmContext_
    void requestDbOpen() throw();

    /// See Rte::Db::Record::FsmContext_
    void requestDbRead() throw();

    /// See Rte::Db::Record::FsmContext_
    void requestDbWrite() throw();

    /// See Rte::Db::Record::FsmContext_
    void verifyOpen() throw();


protected:
    /// See Rte::Db::Record::FsmContext_
    bool isDbBadData() throw();

    /// See Rte::Db::Record::FsmContext_
    bool isDbEof() throw();

    /// See Rte::Db::Record::FsmContext_
    bool isDbError() throw();

    /// See Rte::Db::Record::FsmContext_
    bool isDbSuccess() throw();

    /// See Rte::Db::Record::FsmContext_
    bool isNotCompatible() throw();


protected:
    /// Helper
    void        sendEvent( FSM_EVENT_T msg );

    /// Helper
    void        generateInternalEvent( FSM_EVENT_T msg );

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


};

};      // end namespaces
};
};
#endif  // end header latch
