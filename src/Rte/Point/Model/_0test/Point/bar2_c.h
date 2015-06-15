#ifndef Point_ControllerBar2_h_
#define Point_ControllerBar2_h_
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

#include "Rte/Point/Controller/Base.h"
#include "Rte/Point/Controller/Tuple.h"
#include "Rte/Point/Model/_0test/Point/bar2_m.h"

///
namespace Point {


/** Concrete Controller Point: BAR2
 */
class ControllerBar2: public Bar2,
                      public Rte::Point::Controller::Base
{
public:
    /// Constructor
    ControllerBar2( ModelBar2& modelPoint )
        :Rte::Point::Controller::Base(*this, modelPoint)
            {
            }

};


/** Concrete Tuple Controller Point: BAR2::Foo1
 */
class TupleFoo1ControllerBar2: public Tuple::Foo1, 
                               public Rte::Point::Controller::Tuple
{
public:
    /// Constructor
    TupleFoo1ControllerBar2( ModelBar2& modelPoint )
        :Rte::Point::Controller::Tuple(0, *this, modelPoint)
            {
            }

};


/** Concrete Tuple Controller Point: BAR2::Foo2
 */
class TupleFoo2ControllerBar2: public Tuple::Foo2, 
                               public Rte::Point::Controller::Tuple
{
public:
    /// Constructor
    TupleFoo2ControllerBar2( ModelBar2& modelPoint )
        :Rte::Point::Controller::Tuple(1, *this, modelPoint)
            {
            }

};

};
#endif 