#ifndef Cpl_Rte_Persistence_Record_Base_h_
#define Cpl_Rte_Persistence_Record_Base_h_
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

#include "Cpl/Rte/Persistence/Record/Api_.h"
#include "Cpl/Rte/Persistence/Record/ApiWriter_.h"
#include "Cpl/Rte/Persistence/Record/FsmEventQueue_.h"
#include "Cpl/Rte/Persistence/Record/HandlerApi_.h"
#include "Cpl/Rte/Persistence/Chunk/Handle.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Timer/Local.h"
#include "Cpl/Itc/MailboxServer.h"
#include "Cpl/Log/Loggers.h"
#include <stdint.h>


/// Namespace(s)
namespace Cpl {
namespace Rte {
namespace Persistence {
namespace Record {


/** This class implements the core functionality of a Record Instance.  Each
    Record instance interacts with the Record Handler to perform the initial
    read of the Record data (which is only done once) and writes to the 
    Pesistence storage when the Record's data is modified.
 */
class Base : public Api_,
    public ApiWriter_,
    public FsmEventQueue_
{
protected:
    /// Chunk handle (i.e. contains details about where my data is stored in the DB file)
    Cpl::Rte::Persistence::Chunk::Handle        m_chunkHdl;

    /// My ITC mailbox
    Cpl::Itc::PostApi&                          m_mbox;

    /// Handle to the Record Layer
    Cpl::Rte::Persistence::Record::HandlerApi_* m_recLayerPtr;

    /// Timer for my FSM
    Cpl::Timer::Local<Base>                     m_timer;

    /// Write delay in milliseconds
    unsigned long                               m_writeDelay;

    /// Record/Record name
    Cpl::Container::KeyLiteralString            m_name;

    /// Event Logger
    Cpl::Log::Api&                              m_logger;

    /// Flag that track if the initial read did not match exactly (i.e. the len to less/more than expected)
    bool                                        m_mismatched;

    /// Status of the initial load
    bool                                        m_loadIsGood;

    /// My dirty state (i.e. Record's Model Point data has been modified but the persistent media has not yet been updated)
    bool                                        m_dirty;



public:
    /// Constructor.  Note: Record names must be unique within a Model Database
    Base( Cpl::Container::Map<Api_>&     myRecordList,
          unsigned long                  delayWriteTimeInMsec,
          const char*                    name,
          Cpl::Itc::MailboxServer&       recordLayerMbox,
          Cpl::Log::Api&                 eventLogger = Cpl::Log::Loggers::application()
    );


public:
    /// Cpl::Container::MapItem
    const Cpl::Container::Key& getKey() const throw();



public:
    /// See Cpl::Rte::Persistence::Record::ApiWriter_
    const char* getName( void ) const;

    /// See Cpl::Rte::Persistence::Record::ApiWriter_
    void setChunkHandle( Cpl::Rte::Persistence::Chunk::Handle& src );

    /// See Cpl::Rte::Persistence::Record::ApiWriter_
    void notifyWriteDone( void );


public:
    /// See Cpl::Rte::Persistence::Record::Api_
    void start( HandlerApi_& recordLayer ) throw();

    /// See Cpl::Rte::Persistence::Record::Api_
    void stop() throw();

    /// See Cpl::Rte::Persistence::Record::Api_
    void defaultContent() throw();

    /// See Cpl::Rte::Persistence::Record::Api
    Cpl::Rte::Persistence::Chunk::Handle& getChunkHandle( void );

    /// See Cpl::Rte::Persistence::Record::Api_
    void notifyLoadCompleted( void );

    /// See Cpl::Rte::Persistence::Record::Api_
    void notifyLoadFailed( void );


protected:
    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void connectToModel() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void defaultData() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void disconnectFromModel() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void issueWrite() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void markClean() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void markDirty() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void startTimer() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void stopTimer() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void tellInitialized() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void tellStartCompleted() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void tellStarting() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    void tellStopped() throw();


protected:
    /// See Cpl::Rte::Persistence::Record::FsmContext_
    bool isDirty() throw();

    /// See Cpl::Rte::Persistence::Record::FsmContext_
    bool isLoadGood() throw();


protected:
    /// Helper method: FSM Timer expired
    void  timerExpired( void );
};

};      // end namespaces
};
};
};
#endif  // end header latch
