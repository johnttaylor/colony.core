#ifndef Rte_Element_CoralAddr_h_
#define Rte_Element_CoralAddr_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

//#include "Coral/Definitions.h"
#include "Rte/Element/String.h"

///
namespace Rte { namespace Element {


/** This concrete class provides a concrete implemenation for a Element
    that represents a symbolic CORAL address
 */
//class CoralAddr: public String<OPTION_CORAL_SZ_ADDRESS>
class CoralAddr: public String<10>
{
public:
    /// Constructor
    CoralAddr( const char* initialValue = "",
               bool        inUse        = false,
               int8_t      validState   = RTE_ELEMENT_API_STATE_INVALID
             );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;
};



};      // end namespaces
};
#endif  // end header latch

