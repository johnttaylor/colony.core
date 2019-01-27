#ifndef Rte_Point_Model_ViewerRequest_h_
#define Rte_Point_Model_ViewerRequest_h_
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
#include "Rte/Point/Api.h"


///
namespace Rte { namespace Point { namespace Model {

/** This abstact class define message types and payloads for the set of
    ITC REQUEST services used by a 'Viewer' to query/read-from a Model Point.
 */
class ViewerRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<ViewerRequest> SAP;


public:
    /// Payload for Message: Attach/register for Change Notification
    class RegisterPayload
    {
    public:
        /// The viewer's Point and it is used to the 'new/changed' values
        Rte::Point::Api& m_viewerPoint;

        /// Flag/option on compare values (true) OR use sequence number (false) to trigger a change notification
        bool m_compareValues;

    public:
        /// Constructor
        RegisterPayload( Rte::Point::Api& viewerPoint, bool compareValues=true )
            :m_viewerPoint(viewerPoint)
            ,m_compareValues(compareValues)
                {}
    };

    /** Message Type: Register for a change notification (note: this is a 
        'one-shot' notification. The message can ONLY be used asynchronously
     */
    typedef Cpl::Itc::RequestMessage<ViewerRequest,RegisterPayload> RegisterMsg;
    

public:
    /// Payload for Message: Detaches/un-register for Change Notification
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

    /** Message Type: Cacnel change notification.  The message can ONLY be used
        asynchronously
     */
    typedef Cpl::Itc::RequestMessage<ViewerRequest,CancelPayload> CancelMsg;
    

public:
    /// Payload for Message: Forces a change Notification
    class PollPayload
    {
    public:
    	/** This member refers an RegisterMsg request that needs to be 'polled' 
	        The semantics of 'forced' are VERY similiar to Cancel Message
            will the following exceptions:

            1) returnToSender() will be called on the RegisterMsg (if the
               server still has a reference to it)
            2) The Point contained in the RegisterMsg will be updated before
               it is returned.
            3) The 'poll message' is intended to be called synchronously,
               whereas the Cancel Message is REQUIRED to be called
               asynchronously. 
         */
        RegisterMsg& m_msgToPoll;


    public:
        /// Constructor
        PollPayload( RegisterMsg& msgToForce )
            :m_msgToPoll(msgToForce)
                {}
    };

    /// Message Type: Update Model Point
    typedef Cpl::Itc::RequestMessage<ViewerRequest,PollPayload> PollMsg;



public:
    /// Request: Registers for a change notification
    virtual void request( RegisterMsg& msg ) = 0;
    
    /// Request: Cancels a change notification
    virtual void request( CancelMsg& msg ) = 0;
    
    /// Request: Polls the viewer (i.e. forces the change notification to trigger)
    virtual void request( PollMsg& msg ) = 0;
};



/** This abstact class define message types and payloads for the set of
    ITC RESPONSE services used by a 'Viewer' to query/read-from a Model Point.
 */
class ViewerResponse
{
public:
    /// Response Message Type: Attach (aka change notitfication)
    typedef Cpl::Itc::ResponseMessage<ViewerResponse,
                                      ViewerRequest,
                                      ViewerRequest::RegisterPayload> RegisterMsg;
    
            
    /// Response Message Type: Detach (aka cancel subscription)
    typedef Cpl::Itc::ResponseMessage<ViewerResponse,
                                      ViewerRequest,
                                      ViewerRequest::CancelPayload> CancelMsg;
    
            
public:
    /// Response: Change Notification response
    virtual void response( RegisterMsg& msg ) = 0;

    /// Response: Cancel change notification completed response
    virtual void response( CancelMsg& msg ) = 0;
    
};


};      // end namespaces
};
};
#endif  // end header latch
