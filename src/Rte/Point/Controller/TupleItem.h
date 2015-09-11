#ifndef Rte_Point_Controller_TupleItem_h_
#define Rte_Point_Controller_TupleItem_h_
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

#include "Rte/Point/Controller/Api.h"
#include "Rte/Tuple/Api.h"
#include "Rte/Point/Model/Api.h"

///
namespace Rte { namespace Point { namespace Controller {

/** This template class implements the Controller Point interface 
    for a Point that updates a SINGLE Tuple within a CONTAINER Point.

    The template arg 'TUPLE' is the Tuple type of the Point Container.
    Note: The concrete 'TUPLE' class must inherit from Rte::Tupe::ContainerItem
          per the Point Container semantics/contract.
 */
template <class TUPLE>
class TupleItem: public Api
{
protected:
    /// The client's Tuple data, aka the source Tuple for the write operation
    TUPLE&                      m_myTuple;

    /// Refrence to the Model Point to be updated (aka the destination)
    Rte::Point::Model::Api&     m_modelPoint;

    /// Index of Tuple within the Model Point
    unsigned                    m_tupleIdx;

    /// Remember my changed membership state
    bool                        m_membershipChanged;


protected:
    /// Constructor. 
    TupleItem( unsigned tupleIndex, TUPLE& myTuple, Rte::Point::Model::Api& modelPoint );


public: 
    /// See Rte::Point::Controller::Api
    bool updateModel( void );


public: 
    /** This method is used to 'add' this Tuple to the Point Container.
        The add is done by marking the specified item as "in the container".

        Notes: 
            o The application is still responsible for populating the newly
              enabled item/tuple with meaningful data.
            o The application must still call updateModel() after calling
              addItem() to update the actual model.
            o The call to updateModel() should immediately follow the call
              to this method.
            o If the item/tuple is ALREADY in the container the method
              performs all its operations AND it will cause a
              container membership change notification.
     */
    void add(void);


    /** This method is used to 'remove' this Tuple from the Point Container.
        The remove is done by marking the specified item as NOT "in the
        container".

        Notes: 
            o The application must still call updateModel() after calling
              removeItem() to update the actual model.
            o The call to updateModel() should immediately follow the call
              to this method.
            o If the item/tuple is NOT in the container the method
              performs all its operations AND it will cause a
              container membership change notification.
     */
    void remove(void);

    /** This method changes the index of the TupleItem.  This allows the client
        to 'reuse' a single TupleItem instance for multiple updates to different
        Container indexes. 
     */
    void setIndexTo( unsigned newIndex );

};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class TUPLE>
Rte::Point::Controller::TupleItem<TUPLE>::TupleItem( unsigned tupleIndex, TUPLE& myTuple, Rte::Point::Model::Api& modelPoint )
:m_myTuple(myTuple)
,m_modelPoint(modelPoint)
,m_tupleIdx(tupleIndex)
    {
    }

/////////////////
template<class TUPLE>
bool Rte::Point::Controller::TupleItem<TUPLE>::updateModel( void )
    {
    m_modelPoint.update( m_myTuple, m_tupleIdx, m_membershipChanged );
    m_membershipChanged = false; // Clear the membershp flag incase this Tuple Controller gets reused by the Client 
    return true;
    }


/////////////////
template<class TUPLE>
void Rte::Point::Controller::TupleItem<TUPLE>::add( void )
    {
    m_membershipChanged = true;

    m_myTuple.m_inContainer_.set(true);
    m_myTuple.m_inContainer_.setValid();
    m_myTuple.m_inContainer_.setInUse();
    }

template<class TUPLE>
void Rte::Point::Controller::TupleItem<TUPLE>::remove( void )
    {
    m_membershipChanged = true;

    m_myTuple.m_inContainer_.set(false);
    m_myTuple.m_inContainer_.setValid();
    m_myTuple.m_inContainer_.setInUse();
    }

template<class TUPLE>
void Rte::Point::Controller::TupleItem<TUPLE>::setIndexTo( unsigned newIndex )
    {
    m_tupleIdx = newIndex;
    }

};      // end namespaces
};
};
#endif  // end header latch
