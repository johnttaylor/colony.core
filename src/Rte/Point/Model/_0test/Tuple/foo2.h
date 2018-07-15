#ifndef Tuple_Foo2_h_
#define Tuple_Foo2_h_
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

namespace Tuple {


/** Concrete Tuple: FOO2
 */
class Foo2: public Rte::Tuple::Basic<2>
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::Boolean       m_enabled;
    ///
    Rte::Element::Uinteger8     m_limit;


public:
    /// Constructor
    Foo2( void )
        {
        registerElement( 0, m_enabled );
        registerElement( 1, m_limit );
        }
};

};
#endif
