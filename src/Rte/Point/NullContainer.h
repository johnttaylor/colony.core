#ifndef Rte_Point_NullContainer_h_
#define Rte_Point_NullContainer_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */


#include "Rte/Point/Container.h"
#include "Rte/Tuple/NullItem.h"


///
namespace Rte { namespace Point {


/** This template class implements a "Null Container Point" that contain N 
    tuples. A Null Container Point is ONLY used/needed for the Lite Weight 
    Viewer mechanism. The template parameter 'N' is the number of Null Tuples 
    in the concrete Container Point.
 */
template<int N>
class NullContainer: public Container<Rte::Tuple::NullItem, N>
{
public:
    /// Constructor
    NullContainer( void ){};

};


};      // end namespaces
};
#endif  // end header latch
