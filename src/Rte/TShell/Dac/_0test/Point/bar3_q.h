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
#include "Rte/Point/Query/Tuple.h"
#include "Rte/Point/Query/TupleItem.h"
#include "Rte/TShell/Dac/_0test/Point/bar3.h"

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
            // Default to querying EVERYTHING
            setAllInUseState(true);
            }

};


/** Concrete Tuple Query Point: BAR3::Foo3 (Single Tuple, no traversal)
 */
class TupleFoo3QueryBar3: public Tuple::Foo3, 
                          public Rte::Point::Query::Tuple
{
public:
    /// Constructor
    TupleFoo3QueryBar3( Rte::Point::Model::Api& modelPoint, unsigned tupleIndex, Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY )
        :Rte::Point::Query::Tuple(tupleIndex, *this, modelPoint, copyOption )
            {
            // Default to querying EVERYTHING
            setAllInUseState(true);
            }

};


/** For Tuple QUERY Traversal:

    class MyContext
    {
    protected:
        ///
        Rte::Point::Query::TupleItem<Tuple::Foo3, MyContext> m_tupleWalker;

    
    public:
        /// Constructor
        MyContext( Point::ModelBar3& modelPoint, .... , Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY )
        :m_tupleWalker( modelPoint, *this, &MyContext::walkFoo3, copyOption )
            {
            ....
            m_tupleWalker.setAllInUseState(true);
            m_tupleWalker.issueQuery();
            ...
            }
        

    protectd:
        /// Traversal Callback
        Cpl::Type::Traverser::Status_T walkFoo3( Tuple::Foo3& nextTuple, unsigned tupleIndex );
    };


           
 */

};
#endif 