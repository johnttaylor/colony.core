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
#include "Rte/Point/Query/Tuple.h"
#include "Rte/Point/Model/_0test/Point/bar2_m.h"

///
namespace Point {


/** Concrete Query Point: BAR2
 */
class QueryBar2: public Bar2,
                 public Rte::Point::Query::Base
{
public:
    /// Constructor
    QueryBar2( Rte::Point::Model::Api& modelPoint, Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY )
        :Rte::Point::Query::Base(*this, modelPoint, copyOption )
            {
            }

};


/** Concrete Tuple Query Point: BAR2::Foo1
 */
class TupleFoo1QueryBar2: public Tuple::Foo1, 
                          public Rte::Point::Query::Tuple
{
public:
    /// Constructor
    TupleFoo1QueryBar2( Rte::Point::Model::Api& modelPoint, Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY )
        :Rte::Point::Query::Tuple(0, *this, modelPoint, copyOption)
            {
            }

};

/** Concrete Tuple Query Point: BAR2::Foo2
 */
class TupleFoo2QueryBar2: public Tuple::Foo2, 
                          public Rte::Point::Query::Tuple
{
public:
    /// Constructor
    TupleFoo2QueryBar2( ModelBar2& modelPoint, Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY )
        :Rte::Point::Query::Tuple(1, *this, modelPoint, copyOption)
            {
            }

};

};
#endif 