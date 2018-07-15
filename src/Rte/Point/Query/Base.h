#ifndef Rte_Point_Query_Base_h_
#define Rte_Point_Query_Base_h_
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
#include "Rte/Point/Model/Api.h"


///
namespace Rte { namespace Point { namespace Query {


/** This concrete class implements the Query Point interface.

    NOTE: This Query viewer DEFAULTS the 'in-use' state for all Elements
          to true!
 */
class Base: public Api
{
protected:
    /// Destination to where to stoe the query results
    Rte::Point::Api&         m_myPoint;

    /// The Model Point to read the data from (aka the source)
    Rte::Point::Model::Api&  m_modelPoint;

    /// Query copy operation Option
    Rte::Point::Model::QueryRequest::Option_T m_copyOption;


protected:
    /// Constructor
    Base( Rte::Point::Api&                          myPoint, 
          Rte::Point::Model::Api&                   modelPoint, 
          Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY
        );


public:
    /// See Rte::Point::Query::Api
    bool issueQuery( void );


};


};      // end namespaces
};
};
#endif  // end header latch
