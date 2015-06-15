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
#include "Rte/Tuple/Api.h"
#include "Rte/Point/Model/Api.h"
#include "Cpl/Type/Traverser.h"


///
namespace Rte { namespace Point { namespace Query {


/** This abstract class defines the client interface for walking
    the list of Tuple's in a Container Point, i.e. it defines the 
    callback method for when walking/travering the list of Tuples
    in a Container Point.  
 */
class Traverser
{
public: 
    /// Virtual Destructor
    virtual ~Traverser(){}

public: 
    /** This method is called once for every tuple in the Model point
        being queried.  The contents of the model point's tuple is first
        copied to the client's tuple BEFORE this method is called.  When
        the client has 'found' the tuple it is searching for, this method
        should return continue; else to continue the search the method
        should return eCONTINUE.

        Note: An 'item' in this context is a Tuple. The parameter 'tupleIndex'
              is the zero based index of the Model Point's Tuple that has been
              copied to the client's TupleItem instance.
     */
    virtual Cpl::Type::Traverser::Status_T item( unsigned tupleIndex ) = 0;
};



/** This template class implements the Query Point interface 
    for a Point that reads a SINGLE Tuple within a CONTAINER Point
    and/or traverses - one Tuple at time - the individual Tuples within
    a Container Point..

    The template arg 'TUPLE' is the Tuple type of the Point Container.
    Note: The concrete 'TUPLE' class must inherit from Rte::Tupe::ContainerItem
          per the Point Container semantics/contract.
 */
template <class TUPLE>
class TupleItem: public Api
{
protected:
    /// The client's Tuple data, aka the destination Tuple for the read operation
    TUPLE&                      m_myTuple;

    /// Refrence to the Model Point to be query (aka the source)
    Rte::Point::Model::Api&     m_modelPoint;

    /// Optional Traversal callback
    Traverser*                  m_callbackPtr;

    /// Index of Tuple within the Model Point
    unsigned                    m_tupleIdx;



protected:
    /// Constructor. 
    TupleItem( unsigned tupleIndex, TUPLE& myTuple, Rte::Point::Model::Api& modelPoint, Traverser* walkCallbackPtr =0 );


public: 
    /// See Rte::Point::Query::Api
    void issueQuery( void );


public: 
    /// Returns true if the specified tuple is "in" the container
    bool isItemInContainer( void ) const;

    /** This method allows the client to change/turn-off the traverser callback funciton
     */
    void setWalkerPtr( Traverser* newCallbackPtr );

};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class TUPLE>
Rte::Point::Query::TupleItem<TUPLE>::TupleItem( unsigned tupleIndex, TUPLE& myTuple, Rte::Point::Model::Api& modelPoint, Traverser* walkCallbackPtr )
:m_myTuple(myTuple)
,m_modelPoint(modelPoint)
,m_callbackPtr(walkCallbackPtr)
,m_tupleIdx(tupleIndex)
    {
    }

/////////////////
template<class TUPLE>
void Rte::Point::Query::TupleItem<TUPLE>::issueQuery( void )
    {
    m_modelPoint.query( m_myTuple, m_tupleIdx, m_callbackPtr );
    }


/////////////////
template<class TUPLE>
bool Rte::Point::Query::TupleItem<TUPLE>::isItemInContainer( void ) const
    {
    return m_myTuple.m_inContainer_.get() == true;
    }


template<class TUPLE>
void Rte::Point::Query::TupleItem<TUPLE>::setWalkerPtr( Traverser* newCallbackPtr )
    {
    m_callbackPtr = newCallbackPtr;
    }


};      // end namespaces
};
};
#endif  // end header latch
