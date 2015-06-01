#ifndef Rte_Point_ContainerApi_h_
#define Rte_Point_ContainerApi_h_
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


#include "Rte/Point/Base.h"


///
namespace Rte { namespace Point {




/** This class defines the interface for Container Point
 */
class ContainerApi: public Base
{
public:
    /// Marks the Tuple as 'in the container'
    virtual void addTuple( unsigned tupleIdx ) = 0;

    /// Marks the Tuple as 'NOT in the container
    virtual void removeTuple( unsigned tupleIdx ) = 0;

    /// Returns true if the Tuple is marked as "in the container"
    virtual bool isTupleInContainer( unsigned tupleIdx ) const = 0;

    /** Returns the an index of the next 'in the container' tuple entry.
        The search begins with the specified index. If there a no 
        items in the container, then -1 is returned.
     */
    virtual int nextTuple( unsigned startIdx ) const = 0;

    /** Returns the an index of the next 'available' Tuple entry
        that can be added.  The search begins with the specified
        index. If there a no empty items, then -1 is returned.
     */
    virtual int nextEmptyTuple( unsigned startIdx ) const = 0;

};


};      // end namespaces
};
#endif  // end header latch
