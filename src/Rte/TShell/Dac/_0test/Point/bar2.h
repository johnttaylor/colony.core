#ifndef Point_Bar2_h_
#define Point_Bar2_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

#include "Rte/Point/Basic.h"
#include "Rte/TShell/Dac/_0test/Tuple/foo1.h"
#include "Rte/TShell/Dac/_0test/Tuple/foo2.h"


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
        registerTuple( 0, m_foo1 );
        registerTuple( 1, m_foo2 );
        }

};

};
#endif 
