#ifndef Rte_Tuple_ContainerItem_h_
#define Rte_Tuple_ContainerItem_h_
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


#include "Rte/Element/Basic.h"


///
namespace Rte { namespace Tuple {


/** This base class provides the infra-structure for creating a Tuple that
    can be used in a "Point Container"
 */
class ContainerItem
{
public:
    ///
    Rte::Element::Boolean_T   m_inContainer_;

protected:
    /// Constructor
    ContainerItem( void )
    :m_inContainer_(false)
        {
        }

};



};      // end namespaces
};
#endif  // end header latch
