#ifndef Tuples_h_
#define Tuples_h_
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


/** Concrete Tuple: FOO1
 */
class TupleFoo1: public Rte::Tuple::Basic<5>
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::Boolean_T       m_flag;
    ///
    Rte::Element::String<32>      m_text;
    ///
    Rte::Element::Uinteger16_T    m_data1;
    ///
    Rte::Element::Uinteger16_T    m_data2;
    ///
    Rte::Element::Uinteger8_T     m_enum;

public:
    /// Constructor
    TupleFoo1( void )
        {
        registerElement( 0, m_flag );
        registerElement( 1, m_text );
        registerElement( 2, m_data1 );
        registerElement( 3, m_data2 );
        registerElement( 4, m_enum );
        }
};


/** Concrete Tuple: FOO2 (to be used in Point Container)
 */
class TupleFoo2: public Rte::Tuple::Basic<4>,
                 public Rte::Tuple::ContainerItem
              
            
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::String<10>      m_text;
    ///
    Rte::Element::Boolean_T       m_flag;
    ///
    Rte::Element::Uinteger32_T    m_data;

public:
    /// Constructor
    TupleFoo2( void )
        {
        registerElement( 0, m_inContainer_ );
        registerElement( 1, m_text );
        registerElement( 2, m_flag );
        registerElement( 3, m_data );
        }
};


/** Concrete Tuple: FOO3
 */
class TupleFoo3: public Rte::Tuple::Basic<1>
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::String<10>      m_text;

public:
    /// Constructor
    TupleFoo3( void )
        {
        registerElement( 0, m_text );
        }
};

#endif