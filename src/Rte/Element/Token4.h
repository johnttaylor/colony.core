#ifndef Rte_Element_Token4_h_
#define Rte_Element_Token4_h_
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

#include "Rte/Element/String.h"


///
namespace Rte { namespace Element {


/** This concrete class provides a concrete implemenation for a Element
    that represents a symbolic token that is restricted in length to 4
    characters or less.
 */
class Token4: public String<4>
{
public:
    /// Constructor
    Token4( const char* initialValue = "",
            bool        inUse        = false,
            int8_t      validFlag    = RTE_ELEMENT_API_STATE_INVALID
          );
};



};      // end namespaces
};
#endif  // end header latch

