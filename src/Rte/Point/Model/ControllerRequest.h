#ifndef Rte_Point_Model_ControllerRequest_h_
#define Rte_Point_Model_ControllerRequest_h_
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

#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/SAP.h"
#include "Rte/Point/Api.h"
#include "Rte/Tuple/Api.h"
#include "Rte/Point/Controller/RmwClient.h"


///
namespace Rte { namespace Point { namespace Model {

/** This abstact class define message types and payloads for the set of
    ITC services used by a Controller to update/write-to a Model Point.
 */
class ControllerRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<ControllerRequest> SAP;


public:
    /// Payload for Message: Update Point
    class UpdatePayload
    {
    public:
        /// Application data to be WRITTEN to the Model Point
        Rte::Point::Api& m_srcPoint;

    public:
        ///
        UpdatePayload( Rte::Point::Api& controllerPoint )
            :m_srcPoint(controllerPoint)
                {}
    };

    /// Message Type: Update Model Point
    typedef Cpl::Itc::RequestMessage<ControllerRequest,UpdatePayload> UpdateMsg;
    

public:
    /// Payload for Message: Update a single Tuple in the Point
    class UpdateTuplePayload
    {
    public:
        /// Application data to be WRITTEN to the Model Point
        Rte::Tuple::Api&   m_srcTuple;

        /// Index (within the Point) of the Tuple being updated.
        unsigned           m_tupleIdx;

        /// Flag that indicates that the Tuple update invovles a Point Membership change
        bool               m_membershipChanged;

    public:
        ///
        UpdateTuplePayload( Rte::Tuple::Api& controllerTuple, unsigned tupleIdx, bool membershipChanged=false )
            :m_srcTuple(controllerTuple)
            ,m_tupleIdx(tupleIdx)
            ,m_membershipChanged(membershipChanged) 
                {}
    };

    /// Message Type: Update a single Tuple in the Model Point
    typedef Cpl::Itc::RequestMessage<ControllerRequest,UpdateTuplePayload> UpdateTupleMsg;
    


public:
    /// Payload for Message: Read-Modify-Write a Model Point
    class RmwPayload
    {
    public:
        /// Application Point for the 'Read' operation
        Rte::Point::Api&                   m_clientPoint;

        /// Callback to the client for the modify/write operations
        Rte::Point::Controller::RmwClient& m_clientCb;

    public:
        /// Constructor
        RmwPayload( Rte::Point::Api&                   clientPoint, 
                    Rte::Point::Controller::RmwClient& clientCallback
                  )
            :m_clientPoint(clientPoint)
            ,m_clientCb(clientCallback)
                {}
    
    };
    
    /// Message Type: Read-Modify-Write a Model Point
    typedef Cpl::Itc::RequestMessage<ControllerRequest,RmwPayload> RmwMsg;
    


public:
    /// Payload for Message: Read-Modify-Write a Model Container Point
    class RmwContainerPayload
    {
    public:
        /// Application Tuple for the 'Read' operation
        Rte::Tuple::Api&                            m_clientTuple;

        /// Callback to the client for the modify/write operations
        Rte::Point::Controller::RmwContainerClient& m_clientCb;

        /// Starting Tuple index for the traversal
        unsigned                                    m_tupleIdx;


    public:
        /// Constructor
        RmwContainerPayload( Rte::Tuple::Api&                            clientTuple,
                             Rte::Point::Controller::RmwContainerClient& clientCallback,
                             unsigned                                    startingTupleIndex=0
                           )
            :m_clientTuple(clientTuple)
            ,m_clientCb(clientCallback)
            ,m_tupleIdx(startingTupleIndex)
                {}
    
    };
    
    /// Message Type: Read-Modify-Write a Model Container Point
    typedef Cpl::Itc::RequestMessage<ControllerRequest,RmwContainerPayload> RmwContainerMsg;



public:
    /// Request: Update Model Point
    virtual void request( UpdateMsg& msg ) = 0;
    
    /// Request: Update a single Tuple in the Model Point
    virtual void request( UpdateTupleMsg& msg ) = 0;

    /// Request: Read-Modify-Write a Model Point
    virtual void request( RmwMsg& msg ) = 0;
    
    /// Request: Read-Modify-Write a Model Container Point
    virtual void request( RmwContainerMsg& msg ) = 0;
};


};      // end namespaces
};
};
#endif  // end header latch
