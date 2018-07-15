#ifndef Tuple_Foo1_h_
#define Tuple_Foo1_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

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
    Rte::Element::Boolean       m_enabled;
    ///
    Rte::Element::Uinteger32    m_count;

public:
    /// Constructor
    Foo1( void )
        {
        registerElement( 0, m_name );
        registerElement( 1, m_enabled );
        registerElement( 2, m_count );
        }
};


};
#endif