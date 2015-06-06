#ifndef Tuple_Foo1_h_
#define Tuple_Foo1_h_
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
#include "Rte/Element/Basic.h"
#include "Rte/Element/String.h"

namespace Tuple {

/** Concrete Tuple: FOO1
 */
class Foo1: public Rte::Tuple::Basic<3>
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
    Foo1( void )
        {
        registerElement( m_name );
        registerElement( m_enabled );
        registerElement( m_count );
        endRegistration();
        }
};


};
#endif