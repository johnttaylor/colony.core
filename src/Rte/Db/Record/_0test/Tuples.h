#ifndef Tuples_h_
#define Tuples_h_
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
#include "Rte/Tuple/ContainerItem.h"
#include "Rte/Element/Basic.h"
#include "Rte/Element/String.h"


/** Concrete Tuple: FOO1
 */
class TupleFoo1: public Rte::Tuple::Basic<3>
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::String<1>       m_text1;
    ///
    Rte::Element::String<3>       m_text3;
    ///
    Rte::Element::String<8>       m_text8;

public:
    /// Constructor
    TupleFoo1( void )
        {
        registerElement( 0, m_text1 );
        registerElement( 1, m_text3 );
        registerElement( 2, m_text8 );
        }
};


/** Concrete Tuple: FOO2 (to be used in Point Container)
 */
class TupleFoo2: public Rte::Tuple::Basic<3>,
                 public Rte::Tuple::ContainerItem
              
            
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::String<3>      m_text3;
    ///
    Rte::Element::String<5>      m_text5;

public:
    /// Constructor
    TupleFoo2( void )
        {
        registerElement( 0, m_inContainer_ );
        registerElement( 1, m_text3 );
        registerElement( 2, m_text5 );
        }
};


/** Concrete Tuple: FOO3
 */
class TupleFoo3: public Rte::Tuple::Basic<1>
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::String<10>    m_text10;

public:
    /// Constructor
    TupleFoo3( void )
        {
        registerElement( 0, m_text10 );
        }
};

#endif