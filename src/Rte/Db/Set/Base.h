#ifndef Rte_Db_Set_Base_h_
#define Rte_Db_Set_Base_h_
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
/** @file */

#include "Rte/Db/Set/Api.h"
#include "Rte/Db/Set/ApiLocal.h"
#include "Rte/Db/Set/ApiPoint.h"
#include "Rte/Db/Set/Fsm_.h"
#include "Rte/Db/Set/DefaultRequest.h"
#include "Rte/Db/Set/HandlerLocal.h"
#include "Rte/Db/Record/Handler.h"
#include "Rte/Db/Chunk/Handle.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Timer/Local.h"
#include "Cpl/Log/Loggers.h"
#include <stdint.h>


/// Namespace(s)
namespace Rte { namespace Db { namespace Set { 


/** This class implements the core functionality of a Set Instance.  Each
    Set instance interacts with the Set Handler to perform the initial 
    read of the Set data (which is only done once) and writes to the DB
    when the Set's data is modified.There
 */
class Base: public Api,
            public ApiLocal,
            public ApiPoint,
            public Fsm,
            public DefaultRequest,
            public Rte::Db::Record::Api
{
protected:
    /// Chunk handle (i.e. contains details about where my data is stored in the DB file)
    Rte::Db::Chunk::Handle              m_chunkHdl;

    /// My ITC mailbox
    Cpl::Itc::PostApi&                  m_mbox;

    /// Reference to the Set handler that I belong to
    HandlerLocal&                       m_setHandler;

    /// Handle to the Record Layer
    Rte::Db::Record::Handler*           m_recLayerPtr;

    /// Timer for my FSM
    Cpl::Timer::Local<Base>             m_timer;

    /// Write delay 
    unsigned long                       m_writeDelay;

    /// Set/Record name
    Cpl::Container::KeyLiteralString    m_name;

    /// Event Logger
    Cpl::Log::Api&                      m_logger;

    /// Flag that track if the initial read was 'short' tuples
    bool                                m_missingTuples;

    /// Status of the initial load
    bool                                m_loadIsGood;

    /// My dirty state (i.e. Point data has been modified but the DB media has not yet been updated)
    bool                                m_dirty;

    /// Event Queue (to ensure run-to-completion semantics for the FSM)
    FSM_EVENT_T                         m_queuedEvent;



public:
    /// Constructor
    Base( Cpl::Container::Map<ApiLocal>& mySetList,
          HandlerLocal&                  setHandler, 
          unsigned long                  delayWriteTimeInMsec,
          const char*                    name,
          Cpl::Itc::PostApi&             setLayerMbox,
          Cpl::Timer::CounterSource&     timingSource,  
          Cpl::Log::Api&                 eventLogger = Cpl::Log::Loggers::application()
        );


public:
    /// Cpl::Container::MapItem
    const Cpl::Container::Key& getKey() const throw();


public: 
    /// See Rte::Db::Set::Api
    void defaultSetContent() throw();

    /// See Rte::Db::Set::DefaultRequest
    void request( DefaultMsg& msg );


public: 
    /// See Rte::Db::Record::Api
    const char* getName(void) const;

    /// See Rte::Db::Record::Api
    void setChunkHandle( Rte::Db::Chunk::Handle& src );

    /// See Rte::Db::Record::Api
    Rte::Db::Chunk::Handle& getChunkHandle(void);

    /// See Rte::Db::Record::Api
    bool notifyRead( void* srcBuffer, uint32_t dataLen );

    /// See Rte::Db::Record::Api
    uint32_t fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize );

    /// See Rte::Db::Record::Api
    void notifyWriteDone(void);


public: 
    /// See Rte::Db::Set::ApiLocal
    void start( Rte::Db::Record::Handler& recordLayer ) throw();

    /// See Rte::Db::Set::ApiLocal
    void stop() throw();

    /// See Rte::Db::Set::ApiLocal
    void notifyLoadCompleted(void);

    /// See Rte::Db::Set::ApiLocal
    void notifyLoadFailed(void);

    /// See Rte::Db::Set::ApiLocal
    void defaultContent() throw();

    /// See Rte::Db::Set::ApiLocal
    Rte::Db::Record::Api& getRecord() throw();


protected:
    /// See Rte::Db::Set::FsmContext_
    void connectToModel() throw();

    /// See Rte::Db::Set::FsmContext_
    void defaultSetContents() throw();

    /// See Rte::Db::Set::FsmContext_
    void disconnectFromModel() throw();

    /// See Rte::Db::Set::FsmContext_
    void issueWrite() throw();

    /// See Rte::Db::Set::FsmContext_
    void markClean() throw();

    /// See Rte::Db::Set::FsmContext_
    void markDirty() throw();

    /// See Rte::Db::Set::FsmContext_
    void startTimer() throw();

    /// See Rte::Db::Set::FsmContext_
    void stopTimer() throw();

    /// See Rte::Db::Set::FsmContext_
    void tellInitialized() throw();

    /// See Rte::Db::Set::FsmContext_
    void tellStartCompleted() throw();

    /// See Rte::Db::Set::FsmContext_
    void tellStarting() throw();

    /// See Rte::Db::Set::FsmContext_
    void tellStopped() throw();


protected:
    /// See Rte::Db::Set::FsmContext_
    bool isDirty() throw();

    /// See Rte::Db::Set::FsmContext_
    bool isLoadGood() throw();


protected:
    /// FSM Timer expired
    void  timerExpired( void );

    /// Helper
    void  sendEvent( FSM_EVENT_T msg );

    /// Helper
    void  generateInternalEvent( FSM_EVENT_T msg );



};

};      // end namespaces
};
};
#endif  // end header latch
