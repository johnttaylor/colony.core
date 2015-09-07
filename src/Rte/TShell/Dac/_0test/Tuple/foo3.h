#ifndef Tuple_Foo3_h_
#define Tuple_Foo3_h_
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

#include "Rte/Tuple/Basic.h"
#include "Rte/Tuple/ContainerItem.h"
#include "Rte/Element/Basic.h"
#include "Rte/Element/String.h"

///
namespace Tuple {



/** Concrete Tuple: FOO3
 */
class Foo3: public Rte::Tuple::Basic<4>,
            public Rte::Tuple::ContainerItem
            
            
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::String<10>      m_name;
    ///
    Rte::Element::Boolean_T       m_enabled;
    ///
    Rte::Element::Uinteger32_T    m_count;

public:
    /// Constructor
    Foo3( void )
        {
        registerElement( 0, m_inContainer_ );
        registerElement( 1, m_name );
        registerElement( 2, m_enabled );
        registerElement( 3, m_count );
        }
};

};
#endif 