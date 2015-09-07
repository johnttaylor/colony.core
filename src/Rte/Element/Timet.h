#ifndef Rte_Element_Timet_h_
#define Rte_Element_Timet_h_
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

#include "Rte/Element/Delta.h"
#include <time.h>


///
namespace Rte { namespace Element {

/** This class provide element of the standard C library's time_t type.
    The element is defined as a 'delta' element because most applications
    do not always need a monitor/view time on a per second basis.
 */
class Time_t: public Delta<time_t,DataType::TIMET> 
{
public:
    /// Constructor
    Time_t( time_t  delta        = 0,
            time_t  initialValue = 0,
            bool    inUse        = false,
            int8_t  validState   = RTE_ELEMENT_API_STATE_INVALID
          )
        :Rte::Element::Delta<time_t,DataType::TIMET>(delta,initialValue,inUse,validState)
            {
            }


public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;
};


};      // end namespaces
};
#endif  // end header latch

