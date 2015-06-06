#ifndef Point_Bar1_h_
#define Point_Bar1_h_
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

#include "Rte/Point/Basic.h"
#include "Rte/Point/Model/_0test/Tuple/Foo1.h"

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