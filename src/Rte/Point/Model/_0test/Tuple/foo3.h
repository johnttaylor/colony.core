#ifndef Tuple_Foo3_h_
#define Tuple_Foo3_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Shift-Right Open Repository.  The Repository is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the root directory or on the Internet at           
* http://www.shift-right.com/openrepo/license.htm                               
*                                                                               
* Copyright (c) 2001-2011 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

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
    Foo3( void ):Rte::Tuple::Basic<4>( &m_inContainer_ )
        {
        registerElement( m_name );
        registerElement( m_enabled );
        registerElement( m_count );
        endRegistration();
        }
};

};
#endif 