#ifndef Rte_Point_Query_Container_h_
#define Rte_Point_Query_Container_h_
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
#include "Rte/Point/ContainerApi.h"
#include "Rte/Point/Model/Api.h"


///
namespace Rte { namespace Point { namespace Query {


/** This concrete class implements the Query Point interface 
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

    /// Query copy operation Option
    Rte::Point::Model::QueryRequest::Option_T m_copyOption;


protected:
    /// Constructor
    Container( Rte::Point::ContainerApi&                 myPoint, 
               Rte::Point::Model::Api&                   modelPoint,  
               Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY
             );


public: 
    /// See Rte::Point::Query::Api
    bool issueQuery( void );


public: 
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
