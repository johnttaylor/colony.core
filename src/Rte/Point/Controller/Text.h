#ifndef Rte_Point_Controller_Text_h_
#define Rte_Point_Controller_Text_h_
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

#include "Rte/Point/Controller/Api.h"
#include "Rte/Point/Model/Api.h"
#include "Cpl/Text/String.h"


///
namespace Rte { namespace Point { namespace Controller {


/** This concrete class implements the Controller Point interface that updates
    a point from a generic ASCII text string.  The 'source' format is:

        Point:      "{<tuple>[,<tuple>]*}"

        <tuple>:    "(<elem>[,<elem>*])"

        <elem>:     "[!|^][?[<num>]"
                    "[!|^]<val>"
            
        - Elements/Tuples can be 'skipped' (i.e not updated) by have a blank
          or empty place holder, e.g: "{(,2,),} only updates the 2nd element
          in the first Tuple of the point.
        - The leading '!' and '^' will lock/unlock the element respetively.
        - The leading '?' will set the element to the invalid state.  An 
          optional <num> value (range: 1 - 127) can be specified.  If <num> 
          is not specified the defult INVALID_STATE value is used.
        - <val> can be numeric, symbols, or text.  Text values must be 
          enclosed with quotes.  A Text value is defined as a value that
          can could contain special characters such as commas, spaces, '!', 
          parentheses, etc.    
        - Boolean Element values are encoded as "T" (true) and "F" (false).
        - For a Container Point - the first element in EACH tuple is assumed
          to be the 'in_container" Element - which is Boolean element.  When
          the in_container Element changes values (true->false, false->true)
          it triggers membership change action(s).
       
    
    Examples:

        {(T,3.1415,1), ("My Label",?)}    
        {(T,!3.1415,?32), (!"My Label",!?)}   
        {(T,,"Fred")}


 */
class Text: public Api
{
protected:
    /// Source values to write
    Cpl::Text::String&       m_source;

    /// The Model Point to read the data from (aka the source)
    Rte::Point::Model::Api&  m_modelPoint;

    /// Tuple index
    int                      m_tupleIdx;

   
public:
    /** Constructor.  If 'tupleIndex' is < 0 then the ENTIRE point
        is updated.  If 'tupleIndex' >=0 then only the
        specified tuple is updated. NOTE: Parsing of the 'sourceValues' 
        is destructive, i.e. the orignal content of 'sourceValues' is 
        MODIFIED.
     */

    Text( Cpl::Text::String&       sourceValues,
          Rte::Point::Model::Api&  modelPoint,
          int                      tupleIndex = -1
        );


public:
    /** See Rte::Point::Controller::Api. NOTE: When false is returned the 
        state of the Model Point IS NOT GUARANTEED!
     */
    virtual bool updateModel( void );


};


};      // end namespaces
};
};
#endif  // end header latch
