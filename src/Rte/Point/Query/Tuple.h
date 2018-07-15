#ifndef Rte_Point_Query_Tuple_h_
#define Rte_Point_Query_Tuple_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

#include "Rte/Point/Query/Api.h"
#include "Rte/Tuple/Api.h"
#include "Rte/Point/Model/Api.h"

///
namespace Rte { namespace Point { namespace Query {

/** This concrete class implements the Query Point interface 
    for a Point that reads a SINGLE Tuple within the Point.
 */
class Tuple: public Api
{
protected:
    /// The client's Tuple data, aka the destination Tuple for the read operation
    Rte::Tuple::Api&            m_myTuple;

    /// Refrence to the Model Point to be query (aka the source)
    Rte::Point::Model::Api&     m_modelPoint;

    /// Index of Tuple within the Model Point
    unsigned                    m_tupleIdx;

    /// Query copy operation Option
    Rte::Point::Model::QueryRequest::Option_T m_copyOption;


protected:
    /// Constructor
    Tuple( unsigned                                  tupleIndex, 
           Rte::Tuple::Api&                          myTuple, 
           Rte::Point::Model::Api&                   modelPoint, 
           Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY 
         );


public: 
    /// See Rte::Point::Query::Api
    bool issueQuery( void );


public:
    /** This method allows the client to change the Tuple's index (in the 
        Point). This method SHOULD ONLY BE USED when the Point is Container
        Point - you have been warned!. 
     */
    void setTupleIndex( unsigned newTupleIndex );
};


};      // end namespaces
};
};
#endif  // end header latch
