#ifndef Point_Bar4_h_
#define Point_Bar4_h_
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
#include "Rte/Tuple/Null.h"
#include "Rte/TShell/Dac/_0test/Tuple/foo4.h"

///
namespace Point {



/** Concrete Point: BAR4
 */
class Bar4: public Rte::Point::Basic<1>
{
public:
    ///
    Tuple::Foo4   m_tuple;

public:
    /// Constructor
    Bar4( void )
        {
        registerTuple( 0, m_tuple );
        }

};
        

/** Concrete Null Point: Bar4 (to only be used with Light Weight Viewers)
 */
class Bar4Null: public Rte::Point::Basic<1>
{
public:
    /// 
    Rte::Tuple::Null m_tuple;

public:
    /// Constructor
    Bar4Null( void )
        {
        registerTuple( 0, m_tuple );
        }

};


};
#endif 