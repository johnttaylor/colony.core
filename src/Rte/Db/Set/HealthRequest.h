#ifndef Rte_Db_Set_HealthRequest_h_
#define Rte_Db_Set_HealthRequest_h_
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


#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"


///
namespace Rte { namespace Db { namespace Set {


/** This abstact class define message types and payloads for a set of
    ITC services. These ITC message are used to report the status of the 
    Database with respect to it ability to store data.
 */
class HealthRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<HealthRequest> SAP;

    /// Possible status states
    enum Status_T { eOPENING,                   //!< The DB is in the process of being opened
                    eRUNNING,                   //!< The DB is opened and functional
                    eNO_STORAGE_MEDIA_ERR,      //!< The DB is not storing anything to persistence storage because there an error with the media 
                    eNO_STORAGE_WRONG_SCHEMA,   //!< The DB is not storing anything to persistance storage because of 'major' schema mismatch
                    eCLOSED                     //!< The DB is in the closed state.
                  };

public:
    /// Payload for Message: Reports the DB's current status
    class HealthPayload
    {
    public:
        /// Current DB status
        Status_T    m_status;

    public:
        /// Constructor. 
        HealthPayload( Status_T assumedStatus = eRUNNING ):m_status(assumedStatus){}
    };

    /// Message Type: DB Status
    typedef Cpl::Itc::RequestMessage<HealthRequest,HealthPayload> HealthMsg;


public:
    /// Request: for DB's health
    virtual void request( HealthMsg& msg ) = 0;
    
};


/** This abstact class define message types and payloads for the set of
    ITC RESPONSE services used by the Application to be notified when there
    is change in Set/DB's Health status
 */
class HealthResponse
{
public:
    /// Response Message Type: Health change notification
    typedef Cpl::Itc::ResponseMessage<HealthResponse,
                                      HealthRequest,
                                      HealthRequest::HealthPayload> HealthMsg;
    
            
public:
    /// Response: Health Change Notification response
    virtual void response( HealthMsg& msg ) = 0;
};


};      // end namespaces
};
};
#endif  // end header latch
