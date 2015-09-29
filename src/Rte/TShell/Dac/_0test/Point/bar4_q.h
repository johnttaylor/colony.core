#ifndef Point_QueryBar4_h_
#define Point_QueryBar4_h_
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
#include "Rte/Point/Query/Tuple.h"
#include "Rte/TShell/Dac/_0test/Point/bar4_m.h"

///
namespace Point {


/** Concrete Query Point: BAR4
 */
class QueryBar4: public Bar4,
                 public Rte::Point::Query::Base
{
public:
    /// Constructor
    QueryBar4( Rte::Point::Model::Api& modelPoint, Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY )
        :Rte::Point::Query::Base(*this, modelPoint, copyOption)
            {
            // Default to querying EVERYTHING
            setAllInUseState(true);
            }

};


/** Concrete Tuple Query Point: BAR4::Foo4
 */
class TupleFoo4QueryBar4: public Tuple::Foo4, 
                          public Rte::Point::Query::Tuple
{
public:
    /// Constructor
    TupleFoo4QueryBar4( Rte::Point::Model::Api& modelPoint, Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY  )
        :Rte::Point::Query::Tuple(0, *this, modelPoint, copyOption)
            {
            // Default to querying EVERYTHING
            setAllInUseState(true);
            }

};

};
#endif 