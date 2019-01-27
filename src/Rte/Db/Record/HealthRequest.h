#ifndef Rte_Db_Record_HealthRequest_h_
#define Rte_Db_Record_HealthRequest_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */


#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"


///
namespace Rte { namespace Db { namespace Record {


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
                    eRUNNING_MINOR_UPGRADE,     //!< The DB is opened and functional BUT an 'automatic' minor upgrade was performed during the open process
                    eRUNNING_CORRUPTED_INPUT,   //!< The DB is opened and functional BUT one or more records where corrupt and one or more records were defaulted (records succesfully read before the corruption was detected are NOT defaulted)
                    eNO_STORAGE_MEDIA_ERR,      //!< The DB is opened BUT not storing anything to persistence storage because there an error with the media 
                    eNO_STORAGE_WRONG_SCHEMA,   //!< The DB is opened BUT not storing anything to persistance storage because of 'major' schema mismatch
                    eCLOSING,                   //!< The DB is in the process of being closed
                    eCLOSED,                    //!< The DB is in the closed state.
                    eUNKNOWN                    //!< Unknown status.  This state is typically used by the Monitor for its initial/assumed state
                  };

public:
    /// Payload for Message: Reports the DB's current status
    class RegisterPayload
    {
    public:
        /// Current DB status
        Status_T    m_status;

    public:
        /// Constructor. 
        RegisterPayload( Status_T assumedStatus = eRUNNING ):m_status(assumedStatus){}
    };

    /// Message Type: DB Status
    typedef Cpl::Itc::RequestMessage<HealthRequest,RegisterPayload> RegisterMsg;



public:
    /// Payload for Message: Detaches/un-register for Health Notification
    class CancelPayload
    {
    public:
        /** This member refers an RegisterMsg request that needs to be 
            unregistered  and/or detached.  If the specified message request is
            still  pending, the server will release it reference to the
            RegisterMsg  request (but not call returntoSender() on it), then the
            server will return the Cancel message.  If the RegisterMsg is not
            pending, or is in-transmit back to the client, the response of
            CancelMsg is guarantied to arrive AFTER the in-transmit (or last
            valid change notification) because the message queues are FIFOs,
            i.e. if the client received the Cancel message response,  the server
            no longer has any references to the client's Register messages.
         */
        RegisterMsg& m_msgToCancel;


    public:
        /// Constructor
        CancelPayload( RegisterMsg& msgToCancel )
            :m_msgToCancel(msgToCancel)
                {}
    };

    /** Message Type: Cancel health notification.  The message can ONLY be used
        asynchronously
     */
    typedef Cpl::Itc::RequestMessage<HealthRequest,CancelPayload> CancelMsg;
    


public:
    /// Request: Register for DB's health
    virtual void request( RegisterMsg& msg ) = 0;
    
    /// Request: Cancel registration for DB's health
    virtual void request( CancelMsg& msg ) = 0;
    
};


/** This abstact class define message types and payloads for the set of
    ITC RESPONSE services used by the Application to be notified when there
    is change in Record/DB's Health status
 */
class HealthResponse
{
public:
    /// Response Message Type: Health change notification
    typedef Cpl::Itc::ResponseMessage<HealthResponse,
                                      HealthRequest,
                                      HealthRequest::RegisterPayload> RegisterMsg;
    
public:
    /// Response Message Type: Cancel Health change notification
    typedef Cpl::Itc::ResponseMessage<HealthResponse,
                                      HealthRequest,
                                      HealthRequest::CancelPayload> CancelMsg;
    
            
public:
    /// Response: Health Change Notification response
    virtual void response( RegisterMsg& msg ) = 0;

    /// Response: Cancel Health Change Notification response
    virtual void response( CancelMsg& msg ) = 0;
};


};      // end namespaces
};
};
#endif  // end header latch
