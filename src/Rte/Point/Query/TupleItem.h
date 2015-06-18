#ifndef Rte_Point_Query_TupleItem_h_
#define Rte_Point_Query_TupleItem_h_
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

#include "Rte/Point/Query/Api.h"
#include "Rte/Point/Query/Traverser.h"
#include "Rte/Tuple/Api.h"
#include "Rte/Point/Model/Api.h"


///
namespace Rte { namespace Point { namespace Query {

/** This template class implements the Query Point interface 
    for a Point that reads a SINGLE Tuple within a CONTAINER Point
    and/or traverses - one Tuple at time - the individual Tuples within
    a Container Point..

    The template arg 'TUPLE' is the Tuple type of the Point Container. The
    template arg 'CONTEXT' is a member function in the context class that
    is responsible for processing the traversal callback methods.

    Note: The concrete 'TUPLE' class must inherit from Rte::Tupe::ContainerItem
          per the Point Container semantics/contract.
 */
template <class TUPLE, class CONTEXT>
class TupleItem: public Api,
                 public Traverser,
                 public TUPLE
{
public:
    /** Defines the callback method function for when traversing Tuples within 
        the Container Point.

        This method is called once for every tuple in the Model point being
        queried.  The client can terminate the traversal before the last Tuple
        has been visited by returning eABORT; else the client should return
        eCONTINUE.

        NOTES:
            o The client needs to MINIMIZE the amount of time spend during
              the traversal since all other operations on the Model are blocked
              until the traversal completes (to be precise - the ITC QueryTuple 
              msg must run to completion before the thread its Model Point is
              executing in will process the next ITC message).
               
            o The supplied 'unsiged' argument is the is the zero based index 
              of the 'TUPLE' being visited.  
     */
    typedef Cpl::Type::Traverser::Status_T (CONTEXT::*ItemFunc_T)(TUPLE& /* nextTuple */, unsigned /* tupleIndex */ );



protected:
    /// Refrence to the Model Point to be query (aka the source)
    Rte::Point::Model::Api&     m_modelPoint;

    /// Context that processes the callback
    CONTEXT&                    m_context;

    /// Traversal callback
    ItemFunc_T                  m_itemCb;

    /// Index of Tuple within the Model Point
    unsigned                    m_tupleIdx;

    /// Query copy operation Option
    Rte::Point::Model::QueryRequest::Option_T m_copyOption;             

public:
    /// Constructor. 
    TupleItem( Rte::Point::Model::Api&                   modelPoint, 
               CONTEXT&                                  context,
               ItemFunc_T                                itemCb,
               Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY,
               unsigned                                  tupleIndex = 0 
             );


public: 
    /// See Rte::Point::Query::Api
    void issueQuery( void );


protected:
    /// See Rte::Point::Query::Traverser
    Cpl::Type::Traverser::Status_T item( unsigned tupleIndex );


public: 
    /** This method allows the client to change/set the starting Tuple index 
        in which to start the traversal. The change is applied on the next call
        to issueQuery().
     */
    void setStartIndex( unsigned newStartingIndex );
     
    /** This method allows the client to change/turn-off the traverser callback 
        function. The change is applied on the next call to issueQuery().
     */
    void setCallback( ItemFunc_T newItemCb );

};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class TUPLE, class CONTEXT>
Rte::Point::Query::TupleItem<TUPLE, CONTEXT>::TupleItem( Rte::Point::Model::Api&                   modelPoint, 
                                                         CONTEXT&                                  context,
                                                         ItemFunc_T                                itemCb,
                                                         Rte::Point::Model::QueryRequest::Option_T copyOption,
                                                         unsigned                                  tupleIndex
                                                       )
:m_modelPoint(modelPoint)
,m_context(context)
,m_itemCb(itemCb)
,m_tupleIdx(tupleIndex)
,m_copyOption(copyOption)
    {
    // Default to querying EVERYTHING
    setAllInUseState(true);
    }

/////////////////
template<class TUPLE, class CONTEXT>
void Rte::Point::Query::TupleItem<TUPLE, CONTEXT>::issueQuery( void )
    {
    m_modelPoint.query( *this, m_tupleIdx, this, m_copyOption );
    }


template<class TUPLE, class CONTEXT>
Cpl::Type::Traverser::Status_T Rte::Point::Query::TupleItem<TUPLE, CONTEXT>::item( unsigned tupleIndex )
    {
    return (m_context.*m_itemCb)( *this, tupleIndex );
    }


/////////////////
template<class TUPLE, class CONTEXT>
void Rte::Point::Query::TupleItem<TUPLE, CONTEXT>::setCallback( ItemFunc_T newItemCb )
    {
    m_itemCb = newItemCb;
    }

template<class TUPLE, class CONTEXT>
void Rte::Point::Query::TupleItem<TUPLE, CONTEXT>::setStartIndex( unsigned newStartingIndex )
    {
    m_tupleIdx = newStartingIndex;
    }


};      // end namespaces
};
};
#endif  // end header latch
