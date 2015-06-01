#ifndef Point_ControllerBar2_h_
#define Point_ControllerBar2_h_
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

#include "Rte/Point/Controller/Base.h"
#include "Rte/Point/Model/_0test/Point/Bar2.h"

///
namespace Point {


/** Concrete Controller Point: BAR2
 */
class ControllerBar2: public Bar2,
                      public Rte::Point::Controller::Base
{
public:
    /// Constructor
    ControllerBar2( Rte::Point::Model::Api& modelPoint )
        :Rte::Point::Controller::Base(*this, modelPoint)
            {
            }

};

};
#endif 