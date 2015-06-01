#ifndef Point_Bar3_h_
#define Point_Bar3_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Shift-Right Open Repository.  The Repository is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the root directory or on the Internet at           
* http://www.shift-right.com/openrepo/license.htm                               
*                                                                               
* Copyright (c) 2001-2011 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

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