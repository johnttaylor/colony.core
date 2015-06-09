#ifndef Point_Bar3_h_
#define Point_Bar3_h_
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

#include "Rte/Point/Container.h"
#include "Rte/Point/Model/_0test/Tuple/Foo3.h"

///
namespace Point {



/** Concrete Point: BAR3 (Container Point that hold 4 instances of the Bar3 tuple)
 */
class Bar3: public Rte::Point::Container<Tuple::Foo3,4>
{
public:
    /// Constructor
    Bar3( void ){}

};

};
#endif 