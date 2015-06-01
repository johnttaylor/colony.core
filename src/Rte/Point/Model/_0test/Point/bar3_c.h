#ifndef Point_ControllerBar3_h_
#define Point_ControllerBar3_h_
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

#include "Rte/Point/Controller/Container.h"
#include "Rte/Point/Model/_0test/Point/Bar3.h"

///
namespace Point {


/** Concrete Controller Point: BAR3
 */
class ControllerBar3: public Bar3,
                      public Rte::Point::Controller::Container
{
public:
    /// Constructor
    ControllerBar3( Rte::Point::Model::Api& modelPoint )
        :Rte::Point::Controller::Container(*this, modelPoint)
            {
            }

};

};
#endif 