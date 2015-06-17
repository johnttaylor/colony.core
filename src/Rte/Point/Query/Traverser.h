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




};      // end namespaces
};
};
#endif  // end header latch
