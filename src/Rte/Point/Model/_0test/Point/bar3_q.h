#ifndef Point_QueryBar3_h_
#define Point_QueryBar3_h_
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