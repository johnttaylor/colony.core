#ifndef Point_QueryBar2_h_
#define Point_QueryBar2_h_
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

#include "Rte/Point/Query/Base.h"
#include "Rte/Point/Model/_0test/Point/Bar2.h"

///
namespace Point {


/** Concrete Query Point: BAR2
 */
class QueryBar2: public Bar2,
                 public Rte::Point::Query::Base
{
public:
    /// Constructor
    QueryBar2( Rte::Point::Model::Api& modelPoint )
        :Rte::Point::Query::Base(*this, modelPoint)
            {
            }

};

};
#endif 