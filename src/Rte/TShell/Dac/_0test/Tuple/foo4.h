#ifndef Tuple_Foo4_h_
#define Tuple_Foo4_h_
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

#include "Rte/Tuple/Basic.h"
#include "Rte/Element/Basic.h"
#include "Rte/Element/Delta.h"
#include "Rte/Element/Token4.h"

namespace Tuple {

/** Concrete Tuple: FOO4
 */
class Foo4: public Rte::Tuple::Basic<8>
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::Float           m_single;
    ///
    Rte::Element::Double          m_double;
    ///
    Rte::Element::SizeT            m_sizet;
    ///
    Rte::Element::VoidPtr         m_ptr;
    ///
    Rte::Element::Integer64       m_int64;
    ///
    Rte::Element::Integer8        m_int8;
    ///
    Rte::Element::Uinteger16Delta_t m_uint16;
    ///
    Rte::Element::Token4            m_token4;


public:
    /// Constructor
    Foo4( void )
        {
        registerElement( 0, m_single );
        registerElement( 1, m_double );
        registerElement( 2, m_sizet );
        registerElement( 3, m_ptr );
        registerElement( 4, m_int64 );
        registerElement( 5, m_int8 );
        registerElement( 6, m_uint16 );
        registerElement( 7, m_token4 );
        }
};


};
#endif