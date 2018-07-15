#ifndef Rte_Db_Record_Base_h_
#define Rte_Db_Record_Base_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Base Project.  The Colony.Base Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2018  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Rte/Db/Record/ApiLocal.h"
#include "Rte/Db/Record/ApiLocalWriter.h"
#include "Rte/Db/Record/FsmEventQueue_.h"
#include "Rte/Db/Record/HandlerLocal.h"
#include "Rte/Db/Record/Handler.h"
#include "Rte/Db/Chunk/Handle.h"
#include "Rte/Point/Viewer/Base.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Timer/Local.h"
#include "Cpl/Log/Loggers.h"
#include <stdint.h>


/// Namespace(s)
namespace Rte { namespace Db { namespace Record { 


/** This class implements the core functionality of a Record Instance.  Each
    Record instance interacts with the Record Handler to perform the initial 
    read of the Record data (which is only done once) and writes to the DB
    when the Record's data is modified.There
 */
class Base: public ApiLocal,
            public ApiLocalWriter,
            public FsmEventQueue_,
            public Rte::Point::Viewer::Base
            
{
protected:
    /// Chunk handle (i.e. contains details about where my data is stored in the DB file)
    Rte::Db::Chunk::Handle              m_chunkHdl;

    /// My ITC mailbox
    Cpl::Itc::PostApi&                  m_mbox;

    /// Handle to the Record Layer
    Rte::Db::Record::HandlerLocal*      m_recLayerPtr;

    /// Timer for my FSM
    Cpl::Timer::Local<Base>             m_timer;

    /// Write delay 
    unsigned long                       m_writeDelay;

    /// Record/Record name
    Cpl::Container::KeyLiteralString    m_name;

    /// Event Logger
    Cpl::Log::Api&                      m_logger;

    /// Flag that track if the initial read was 'short' tuples
    bool                                m_missingTuples;

    /// Status of the initial load
    bool                                m_loadIsGood;

    /// My dirty state (i.e. Point data has been modified but the DB media has not yet been updated)
    bool                                m_dirty;



public:
    /// Constructor
    Base( Rte::Point::Api&               myNullPoint4Viewer,
          Rte::Point::Model::Api&        modelPoint,
          Cpl::Container::Map<ApiLocal>& myRecordList,
          unsigned long                  delayWriteTimeInMsec,
          const char*                    name,
          Cpl::Itc::PostApi&             recordLayerMbox,
          Cpl::Timer::CounterSource&     timingSource,  
          Cpl::Log::Api&                 eventLogger = Cpl::Log::Loggers::application()
        );


public:
    /// Cpl::Container::MapItem
    const Cpl::Container::Key& getKey() const throw();



public: 
    /// See Rte::Db::Record::ApiLocalWriter
    const char* getName(void) const;

    /// See Rte::Db::Record::ApiLocalWriter
    void setChunkHandle( Rte::Db::Chunk::Handle& src );

    /// See Rte::Db::Record::ApiLocalWriter
    uint32_t fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize );

    /// See Rte::Db::Record::ApiLocalWriter
    void notifyWriteDone(void);


public: 
    /// See Rte::Db::Record::ApiLocal
    void start( HandlerLocal& recordLayer ) throw();

    /// See Rte::Db::Record::ApiLocal
    void stop() throw();

    /// See Rte::Db::Record::ApiLocal
    void defaultContent() throw();

    /// See Rte::Db::Record::Api
    Rte::Db::Chunk::Handle& getChunkHandle(void);

    /// See Rte::Db::Record::Api
    bool notifyRead( void* srcBuffer, uint32_t dataLen );

    /// See Rte::Db::Record::ApiLocal
    void notifyLoadCompleted(void);

    /// See Rte::Db::Record::ApiLocal
    void notifyLoadFailed(void);


protected: 
    /// See Rte::Point::Viewer::Base
    void modelHasChanged( void );

    /// See Rte::Point::Viewer::Base
    void viewerHasStopped( void );

protected:
    /// See Rte::Db::Record::FsmContext_
    void connectToModel() throw();

    /// See Rte::Db::Record::FsmContext_
    void defaultData() throw();

    /// See Rte::Db::Record::FsmContext_
    void disconnectFromModel() throw();

    /// See Rte::Db::Record::FsmContext_
    void issueWrite() throw();

    /// See Rte::Db::Record::FsmContext_
    void markClean() throw();

    /// See Rte::Db::Record::FsmContext_
    void markDirty() throw();

    /// See Rte::Db::Record::FsmContext_
    void startTimer() throw();

    /// See Rte::Db::Record::FsmContext_
    void stopTimer() throw();

    /// See Rte::Db::Record::FsmContext_
    void tellInitialized() throw();

    /// See Rte::Db::Record::FsmContext_
    void tellStartCompleted() throw();

    /// See Rte::Db::Record::FsmContext_
    void tellStarting() throw();

    /// See Rte::Db::Record::FsmContext_
    void tellStopped() throw();


protected:
    /// See Rte::Db::Record::FsmContext_
    bool isDirty() throw();

    /// See Rte::Db::Record::FsmContext_
    bool isLoadGood() throw();


protected:
    /// FSM Timer expired
    void  timerExpired( void );


};

};      // end namespaces
};
};
#endif  // end header latch
