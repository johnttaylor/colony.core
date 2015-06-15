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
    /// Request: Update Model Point
    virtual void request( UpdateMsg& msg ) = 0;
    
    /// Request: pdate a single Tuple in the Model Point
    virtual void request( UpdateTupleMsg& msg ) = 0;
};


};      // end namespaces
};
};
#endif  // end header latch
