#ifndef Rte_Point_Query_Text_h_
#define Rte_Point_Query_Text_h_
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

#include "Rte/Point/Query/Api.h"
#include "Rte/Point/Model/Api.h"
#include "Cpl/Text/String.h"


///
namespace Rte { namespace Point { namespace Query {


/** This concrete class implements the Query Point interface that returns
    the result in a generic ASCII text string.  The result format is:

    "{(<tuple>)[,(<tuple>)]*}"

    where <tuple>: elem[,elem]* 

        - All 'text' element are bounded within quotes.  'text' elements are
          defined as elements whos ASCII text values could contain special
          characters such as commas, spaces, '!', parentheses, etc.    
        - Boolean values are encoded as "T" (true) and "F" (false)
        - If the element is in the locked state, then it is prefixed with
          an '!' character
        - If an element is invalid, then "?<num> is returned as the value
          where <num> is the invalid state value (1-127).  If <num> equals
          the default INVALID_STATE value then just "?" is returned.
        - For a Container Point - the first element in EACH tuple is assumed
          to be the 'in_container" Element is as such is bounded by '[]'
       
    
    Examples:

        {(T,3.1415,1), ("My Label",?)}    
        {(T,!3.1415,?32), (!"My Label",!?)}   
        {([T],32,"Fred")}


 */
class Text: public Api
{
protected:
    /// Destination to where to store the query results
    Cpl::Text::String&       m_results;

    /// The Model Point to read the data from (aka the source)
    Rte::Point::Model::Api&  m_modelPoint;

    /// Tuple index
    int                      m_tupleIdx;

   
public:
    /** Constructor.  If 'tupleIndex' is < 0 then the ENTIRE point
        is queried/returned.  If 'tupleIndex' >=0 then only the
        specified tuple is queired/returned.
     */

    Text( Cpl::Text::String&       storageForResults,
          Rte::Point::Model::Api&  modelPoint,
          int                      tupleIndex = -1
        );


public:
    /// See Rte::Point::Query::Api
    bool issueQuery( void );


};


};      // end namespaces
};
};
#endif  // end header latch
