#ifndef Rte_Point_Model_QueryRequest_h_
#define Rte_Point_Model_QueryRequest_h_
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
#include "Rte/Point/Query/Traverser.h"
#include "Rte/Tuple/Api.h"

///
namespace Rte { namespace Point { namespace Model {

/** This abstact class define message types and payloads for the set of
    ITC services used by a 'Viewer' to query/read-from a Model Point.
 */
class QueryRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<QueryRequest> SAP;


public:
    /// Query options
    enum Option_T { eCOPY,                       //!< Unconditional copy of Model Point
                    eCOMPARE_VALUES_AND_COPY,    //!< Only copy Tuples when the values are different
                    eCOMPARE_SEQNUM_AND_COPY     //<! Only copy Tuples when their sequence numbers are different
                  };

public:
    /// Payload for Message: Query Point
    class QueryPayload
    {
    public:
        /// Destination for where the Model Point's data will be COPIED to
        Rte::Point::Api& m_dstPoint;

        /// Query Option
        Option_T         m_copyOption;


    public:
        ///
        QueryPayload( Rte::Point::Api& queryPoint, Option_T copyOption =eCOPY )
            :m_dstPoint(queryPoint)
            ,m_copyOption(copyOption)
                {}
    };

    /// Message Type: Update Model Point
    typedef Cpl::Itc::RequestMessage<QueryRequest,QueryPayload> QueryMsg;
    

public:
    /// Payload for Message: Query for a single Tuple in the Point
    class QueryTuplePayload
    {
    public:
        /// Destination for where the Model Point Tuple's data will be COPIED to
        Rte::Tuple::Api&                m_dstTuple;

        /// Index (within the Point) of the Tuple being queried.
        unsigned                        m_tupleIdx;

        /// Callback Pointer to traversing a Container Point
        Rte::Point::Query::Traverser*   m_callbackPtr;

        /// Query Option
        Option_T                        m_copyOption;


    public:
        ///
        QueryTuplePayload( Rte::Tuple::Api& queryTuple, unsigned tupleIdx, Rte::Point::Query::Traverser* callbackPtr = 0, Option_T copyOption =eCOPY )
            :m_dstTuple(queryTuple)
            ,m_tupleIdx(tupleIdx) 
            ,m_callbackPtr(callbackPtr)
            ,m_copyOption(copyOption)
                {}
    };

    /// Message Type: Update a single Tuple in the Model Point
    typedef Cpl::Itc::RequestMessage<QueryRequest,QueryTuplePayload> QueryTupleMsg;
    


public:
    /// Request: Reads a Model Point
    virtual void request( QueryMsg& msg ) = 0;
    
    /// Request: Reads a single Tuple in the Model Point
    virtual void request( QueryTupleMsg& msg ) = 0;
};


};      // end namespaces
};
};
#endif  // end header latch
