#ifndef Point_Bar1_h_
#define Point_Bar1_h_
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

#include "Rte/Point/Basic.h"
#include "Rte/Point/Model/_0test/Tuple/foo1.h"

///
namespace Point {



/** Concrete Point: BAR1
 */
class Bar1: public Rte::Point::Basic<1>
{
public:
    ///
    Tuple::Foo1   m_tuple;

public:
    /// Constructor
    Bar1( void )
        {
        registerTuple( m_tuple );
        endRegistration();
        }

};

};
#endif 