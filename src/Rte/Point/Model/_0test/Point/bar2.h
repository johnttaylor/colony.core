#ifndef Point_Bar2_h_
#define Point_Bar2_h_
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
#include "Rte/Point/Model/_0test/Tuple/Foo2.h"


///
namespace Point {


/** Concrete Point: BAR2
 */
class Bar2: public Rte::Point::Basic<2>
{
public:
    ///
    Tuple::Foo1   m_foo1;
    ///
    Tuple::Foo2   m_foo2;


public:
    /// Constructor
    Bar2( void )
        {
        setTupleIndex( m_foo1, 0 );
        setTupleIndex( m_foo2, 1 );
        }

};

};
#endif 
