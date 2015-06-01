#ifndef Rte_Point_Controller_Container_h_
#define Rte_Point_Controller_Container_h_
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
#include "Rte/Point/ContainerApi.h"
#include "Rte/Point/Model/Api.h"

///
namespace Rte { namespace Point { namespace Controller {

/** This concrete class implements the Controller Point interface 
    for a Point that is container. Note: An "item" in the container 
    is a Tuple.
 */
class Container: public Api
{
protected:
    /// The client's Point data, aka the source point for the write operation
    Rte::Point::ContainerApi&   m_myPoint;

    /// Refrence to the Model Point to be updated (aka the destination)
    Rte::Point::Model::Api&     m_modelPoint;


protected:
    ///
    Container( Rte::Point::ContainerApi& myPoint, Rte::Point::Model::Api& modelPoint );


public: 
    /// See Rte::Point::Controller::Api
    void updateModel( void );


public: 
    /** This method is used to 'add' a item/tuple to the Point container.
        The add is done by marking the specified item as "in the container".

        Notes: 
            o The application is still responsible for populating the newly
              enabled item/tuple with meaningful data.
            o The application must still call updateModel() after calling
              addItem() to update the actual model.
            o If the item/tuple is ALREADY in the container the method
              performs all its operations AND it will cause a
              container membership change notification.
     */
    inline void addItem( unsigned tupleIdx )                 { m_myPoint.addTuple( tupleIdx ); }

    /** This method is used to 'remove' a item/tuple from the Point container.
        The remove is done by marking the specified item as NOT "in the
        container".

        Notes: 
            o The application must still call updateModel() after callomg
              removeItem() to update the actual model.
            o If the item/tuple is NOT in the container the method
              performs all its operations AND it will cause a
              container membership change notification.
     */
    inline void removeItem( unsigned tupleIdx )              { m_myPoint.removeTuple( tupleIdx ); }

    /// Returns true if the specified tuple is "in" the container
    inline bool isItemInContainer( unsigned tupleIdx ) const { return m_myPoint.isTupleInContainer( tupleIdx ); }

    /** Returns the an index of the next 'in the container' tuple entry.
        The search begins with the specified index. If there a no 
        items in the container, then -1 is returned.
     */
    inline int nextItem( unsigned startIdx ) const      { return m_myPoint.nextTuple(startIdx); }

    /** Returns the an index of the next 'available' tuple entry
        that can be added.  The search begins with the specified
        index. If there a no empty items, then -1 is returned.
     */
    inline int nextEmptyItem( unsigned startIdx ) const { return m_myPoint.nextEmptyTuple(startIdx); }

};


};      // end namespaces
};
};
#endif  // end header latch
