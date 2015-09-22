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
#include "Rte/Tuple/NullItem.h"
#include "Rte/TShell/Dac/_0test/Tuple/foo3.h"

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

/** Concrete Null Point: BAR3 (to only be used with Light Weight Viewers)
 */
class Bar3Null: public Rte::Point::Container<Rte::Tuple::NullItem,4>
{
public:
    /// Constructor
    Bar3Null( void ){}

};


};
#endif 