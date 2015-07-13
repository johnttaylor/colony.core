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
#include "Rte/Db/Set/Request.h"
#include "Rte/Db/Record/Client.h"
#include "Rte/Db/Set/LocalHandler_.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Set { 


/** This class implements the Set Handler
 */
class Server: public Handler,
              public LocalHandler_,
              public Request,
              public Rte::Db::Record::Client
{
protected:
    /// Handle to the Record Layer
    Rte::Db::Record::Handler&   m_recordLayer;

    /// My opened/closed state
    bool                        m_opened;

    /// Remember if at least one set performed a 'conversion'
    bool                        m_conversion;

    /// Number of Sets that have NOT been succesfully initialized
    unsigned                    m_setCount;

    /// Number of started/opened Sets
    unsigned                    m_openCount;

    /// Number of stopped/closed Sets
    unsigned                    m_closeCount;

    /// Cache my Open Request message
    OpenMsg*                    m_openMsgPtr;

    /// Cache my Close Request message
    CloseMsg*                   m_closeMsgPtr;

    /// Hash table for quick look-up of Record names
    Cpl::Container::Dictionary<Rte::Db::Set::LocalApi_>& m_sets;


public:
    /// Constructor
    Server( Rte::Db::Record::Handler&                              recordLayer,
            Cpl::Itc::PostApi&                                     setAndRecordLayersMbox, 
            Cpl::Container::Dictionary<Rte::Db::Set::LocalApi_>&   sets
          );


public:
    /// See Rte::Db::Set::Handler
    void defaultAllSetsContent() throw();

    /// See Rte::Db::Set::Request
    void request( DefaultMsg& msg );

    /// See Cpl::Itc::Open
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /// See Cpl::Itc::Close
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


public:
    /// See Rte::Db::Set::LocalHandler_
    void notifySetWaiting( LocalApi_& set );

    /// See Rte::Db::Set::LocalHandler_
    void notifySetInitialized( LocalApi_& set );

    /// See Rte::Db::Set::LocalHandler_
    void notifySetConverted( LocalApi_& set );

    /// See Rte::Db::Set::LocalHandler_
    void notifySetStopped( LocalApi_& set );

    /// See Rte::Db::Set::LocalHandler_
    void notifySetStarted( LocalApi_& set );

    /// See Rte::Db::Set::LocalHandler_
    void registerSet( LocalApi_& set );


public:
    /// See Rte::Db::Record::Client
    Rte::Db::Record::Api* getRecordApi( const char* recName, uint16_t nameLen );

    /// See Rte::Db::Record::Client
    void notifyOpenCompleted(void);

    /// See Rte::Db::Record::Client
    void notifyOpenedWithErrors(void);

    /// See Rte::Db::Record::Client
    void notifyIncompatible(void);

    /// See Rte::Db::Record::Client
    bool isCleanLoad(void);

    /// See Rte::Db::Record::Client
    void notifyStopped();
};


};      // end namespaces
};
};
#endif  // end header latch
