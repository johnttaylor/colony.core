#ifndef Rte_Point_Query_Traverser_h_
#define Rte_Point_Query_Traverser_h_
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
    /** This method is called once for every tuple in the Model point being 
        queried.  This method is called once for every tuple in the Model
        point being queried.  The contents of the Model point's tuple is first
        copied to the client's Tuple BEFORE this method is called. The client
        can terminate the traversal by returning eABORT; else return eCONTINUE
        for the next tuple.

        NOTES:
            o The client needs to MINIMIZE the amount of time spend during
              the traversal since all other operations on the Model are blocked
              until the traversal completes (to be precise - the ITC QueryTuple 
              msg must run to completion before the thread its Model Point is
              executing in will process the next ITC message).
               
            o The supplied 'tupleIndex' argument is the is the zero based index 
              of the 'TUPLE' being visited.  The client can identify that
              last tuple in traversal when 'tupleIndex' == number-of-tuples - 1

     */
    virtual Cpl::Type::Traverser::Status_T item( unsigned tupleIndex ) = 0;
};




};      // end namespaces
};
};
#endif  // end header latch
