#ifndef Cpl_Rte_Persistence_Record_Base_h_
#define Cpl_Rte_Persistence_Record_Base_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Base Project.  The Colony.Base Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Dm/Persistence/Chunk/Handle.h"
#include "Cpl/Dm/Persistence/Record/Api_.h"
#include "Cpl/Dm/Persistence/Record/ApiWriter_.h"
#include "Cpl/Dm/Persistence/Record/FsmEventQueue_.h"
#include "Cpl/Dm/Persistence/Record/HandlerApi_.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/ModelPoint.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Container/SList.h"
#include "Cpl/System/Timer.h"
#include <stdint.h>


/// Namespace(s)
namespace Cpl {
namespace Dm {
namespace Persistence {
namespace Record {


/** This class implements the core functionality of a Record Instance.  Each
    Record instance interacts with the Record Handler to perform the initial
    read of the Record data (which is only done once) and writes to the
    Persistence storage when the Record's data is modified.

    The concrete Record classes to call registerModelPoint() - for each of
    their Model Points - in their constructor.
    
    The concrete Record classes are required to the implemented the following
    methods:
            void defaultData() throw()
            void connectToModel() throw()
            void disconnectFromModel() throw()

    The defaultData() method is responsible for updating all of the concrete 
    Record's Model Point's to the appropriate default values.  Note: A Model 
    Point can be defaulted to the "invalid state" since each Model Point's 
    valid/invalid state is stored as part of the persistent record.

    The connectToModel() method is responsible for registering each individual
    model point for change notifications.  In the callback for the change 
    notifications, the concrete class is required to generate the evDataModified
    event.  When registering for the change notification, the concrete class 
    needs to register with the MP's current sequence number (do NOT use 'UNKNOWN'
    for the sequence number value. Note: No other actions are required in the 
    callback methods.

    The disconnectFromModel() method is responsible for canceling the change
    notification registrations.
 */
class Base :
    public Api_,
    public ApiWriter_,
    public FsmEventQueue_
{
public:
    /// Type for a 'private' list of Model Points (See Cpl::Container namespace for constraints about items being in more than one list)
    typedef Cpl::Container::ReferenceItem<Cpl::Dm::ModelPoint, Cpl::Container::Item> ModelPointReference_t;

protected:
    /// List of model points in the Record
    Cpl::Container::SList<ModelPointReference_t> m_points;

    /// Chunk handle (i.e. contains details about where my data is stored in the DB file)
    Cpl::Dm::Persistence::Chunk::Handle        m_chunkHdl;

    /// My ITC mailbox
    Cpl::Dm::MailboxServer&                    m_mbox;

    /// Handle to the Record Layer
    Cpl::Dm::Persistence::Record::HandlerApi_* m_recLayerPtr;

    /// Timer for my FSM
    Cpl::Timer::Local<Base>                     m_timer;

    /// Write delay in milliseconds
    unsigned long                               m_writeDelay;

    /// Record/Record name
    Cpl::Container::KeyLiteralString            m_name;

    /// Event Logger
    Cpl::Log::Api&                              m_logger;

    /// Flag that track if the initial read did not match exactly (i.e. the record length was less/more than expected)
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
          Cpl::Dm::MailboxServer&       recordLayerMbox,
          Cpl::Log::Api&                 eventLogger = Cpl::Log::Loggers::application()
    );


public:
    /// Cpl::Container::MapItem
    const Cpl::Container::Key& getKey() const throw();



public:
    /// See Cpl::Dm::Persistence::Record::ApiWriter_
    const char* getName( void ) const;

    /// See Cpl::Dm::Persistence::Record::ApiWriter_
    void setChunkHandle( Cpl::Dm::Persistence::Chunk::Handle& src );

    /// See Cpl::Dm::Persistence::Record::ApiWriter_
    void notifyWriteDone( void );

    /// See Cpl::Dm::Persistence::Record::ApiWriter_
    uint32_t fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize );

public:
    /// See Cpl::Dm::Persistence::Record::Api_
    void start( HandlerApi_& recordLayer ) throw();

    /// See Cpl::Dm::Persistence::Record::Api_
    void stop() throw();

    /// See Cpl::Dm::Persistence::Record::Api_
    void defaultContent() throw();

    /// See Cpl::Dm::Persistence::Record::Api
    Cpl::Dm::Persistence::Chunk::Handle& getChunkHandle( void );

    /// See Cpl::Dm::Persistence::Record::Api_
    bool notifyRead( void* srcBuffer, uint32_t dataLen );

    /// See Cpl::Dm::Persistence::Record::Api_
    void notifyLoadCompleted( void );

    /// See Cpl::Dm::Persistence::Record::Api_
    void notifyLoadFailed( void );


protected:
    /// See Cpl::Dm::Persistence::Record::FsmContext_
    void issueWrite() throw();

    /// See Cpl::Dm::Persistence::Record::FsmContext_
    void markClean() throw();

    /// See Cpl::Dm::Persistence::Record::FsmContext_
    void markDirty() throw();

    /// See Cpl::Dm::Persistence::Record::FsmContext_
    void startTimer() throw();

    /// See Cpl::Dm::Persistence::Record::FsmContext_
    void stopTimer() throw();

    /// See Cpl::Dm::Persistence::Record::FsmContext_
    void tellInitialized() throw();

    /// See Cpl::Dm::Persistence::Record::FsmContext_
    void tellStartCompleted() throw();

    /// See Cpl::Dm::Persistence::Record::FsmContext_
    void tellStarting() throw();

    /// See Cpl::Dm::Persistence::Record::FsmContext_
    void tellStopped() throw();


protected:
    /// See Cpl::Dm::Persistence::Record::FsmContext_
    bool isDirty() throw();

    /// See Cpl::Dm::Persistence::Record::FsmContext_
    bool isLoadGood() throw();

protected:
    /** This method is used by the concrete Record class to 'register' all of
        its Model Point(s).

        NOTE: ORDER IS IMPORTANT - the first register MP is the first MP stored
              in the record.  This means that if the application/child class
              changes the registration order - then on next start-up cycle
              the expected order and the order in media won't match and the
              record will be defaulted (i.e. lose its persistent values)
     */
    void registerModelPoint( ModelPointReference_t& mpToAdd );

protected:
    /// Helper method: FSM Timer expired
    void  timerExpired( void );
};

};      // end namespaces
};
};
};
#endif  // end header latch
