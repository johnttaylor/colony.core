#ifndef Tuple_Foo2_h_
#define Tuple_Foo2_h_
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

namespace Tuple {


/** Concrete Tuple: FOO2
 */
class Foo2: public Rte::Tuple::Basic<2>
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::Boolean_T       m_enabled;
    ///
    Rte::Element::Uinteger8_T     m_limit;


public:
    /// Constructor
    Foo2( void )
        {
        registerElement( m_enabled );
        registerElement( m_limit );
        endRegistration();
        }
};

};
#endif
