#ifndef Rte_Db_Set_Server_h_
#define Rte_Db_Set_Server_h_
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

#include "Rte/Db/Set/Handler.h"
#include "Rte/Db/Set/DefaultRequest.h"
#include "Rte/Db/Set/HealthRequest.h"
#include "Rte/Db/Record/Client.h"
#include "Rte/Db/Record/Handler.h"
#include "Rte/Db/Set/HandlerLocal.h"
#include "Cpl/Container/DList.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Itc/CloseSync.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Set { 


/** This class implements the Set Handler

    NOTE: The ITC open request will return false if the DB ends up in the 
          'No Persistence' state during the opening process.

 */
class Server: public Handler,
              public HandlerLocal,
              public HealthRequest,
              public DefaultRequest,
              public Rte::Db::Record::Client,
              public Cpl::Itc::CloseSync
{
protected:
    /// List of Clients monitor my health
    Cpl::Container::DList<HealthMsg>    m_pendingHealth;

    /// SAP for my Viewer ITC interface
    HealthRequest::SAP                  m_healthSAP;

    /// Handle to the Record Layer
    Rte::Db::Record::Handler&           m_recordLayer;

    /// My opened/closed state
    bool                                m_opened;

    /// Remember if at least one set performed a 'conversion'
    bool                                m_conversion;

    /// Remember if a 'no persistence' error occurred
    bool                                m_noPersistence;

    /// Number of Sets that have NOT been succesfully initialized
    unsigned                            m_setCount;

    /// Number of started/opened Sets
    unsigned                            m_openCount;

    /// Number of stopped/closed Sets
    unsigned                            m_closeCount;
                                        
    /// Cache my Open Request message
    OpenMsg*                            m_openMsgPtr;

    /// Cache my Close Request message
    CloseMsg*                           m_closeMsgPtr;

    /// Current Health
    HealthRequest::Status_T             m_status;


    /// Map containing reference to all Sets for assocaited DB instance
    Cpl::Container::Map<Rte::Db::Set::ApiLocal>& m_sets;


public:
    /// Constructor
    Server( Rte::Db::Record::Handler&                      recordLayer,
            Cpl::Itc::PostApi&                             setAndRecordLayersMbox, 
            Cpl::Container::Map<Rte::Db::Set::ApiLocal>&   sets
          );


public:
    /// See Rte::Db::Set::Handler
    HealthRequest::SAP&  getHealthSAP(void);

    /// See Rte::Db::Set::Handler
    void defaultAllSetsContent() throw();

    /// See Rte::Db::Set::DefaultRequest
    void request( HealthMsg& msg );

    /// See Rte::Db::Set::DefaultRequest
    void request( DefaultMsg& msg );

    /// See Cpl::Itc::Open
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /// See Cpl::Itc::Close
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );



public:
    /// See Rte::Db::Set::HandlerLocal
    void notifySetWaiting( ApiLocal& set );

    /// See Rte::Db::Set::HandlerLocal
    void notifySetInitialized( ApiLocal& set );

    /// See Rte::Db::Set::HandlerLocal
    void notifySetConverted( ApiLocal& set );

    /// See Rte::Db::Set::HandlerLocal
    void notifySetStopped( ApiLocal& set );

    /// See Rte::Db::Set::HandlerLocal
    void notifySetStarted( ApiLocal& set );


public:
    /// See Rte::Db::Record::Client
    Rte::Db::Record::Api* getRecordApi( const char* recName, uint16_t nameLen );

    /// See Rte::Db::Record::Client
    void notifyOpenCompleted(void);

    /// See Rte::Db::Record::Client
    void notifyOpenedWithErrors( Rte::Db::Record::Client::OpenError_T errorCode );

    /// See Rte::Db::Record::Client
    bool isCleanLoad(void);

    /// See Rte::Db::Record::Client
    void notifyWriteError(void);

    /// See Rte::Db::Record::Client
    void notifyStopped();


protected:
    /// Helper
    void updateHealthStatus( Rte::Db::Record::Client::OpenError_T openErrorCode );

    /// Helper
    void setNewHealthStatus( HealthRequest::Status_T newstatus );

};


};      // end namespaces
};
};
#endif  // end header latch
