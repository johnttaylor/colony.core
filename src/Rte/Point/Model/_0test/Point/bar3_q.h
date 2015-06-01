#ifndef Point_QueryBar3_h_
#define Point_QueryBar3_h_
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

#include "Rte/Point/Query/Container.h"
#include "Rte/Point/Model/_0test/Point/Bar3.h"

///
namespace Point {


/** Concrete Query Point: BAR3
 */
class QueryBar3: public Bar3,
                 public Rte::Point::Query::Container
{
public:
    /// Constructor
    QueryBar3( Rte::Point::Model::Api& modelPoint )
        :Rte::Point::Query::Container(*this, modelPoint)
            {
            }

};

};
#endif 