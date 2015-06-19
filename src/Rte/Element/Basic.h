#ifndef Rte_Element_Basic_h_
#define Rte_Element_Basic_h_
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

#include "Rte/Element/Base.h"
#include <memory.h>
#include <stdint.h>

///
namespace Rte { namespace Element {


/** This template class provides a concrete implemenation for a Element
    that has data content of the C primitive type 'ELEMTYPE'. The
    'TYPEID' template argument is the DataType ID for the Element 
 */
template<class ELEMTYPE, int TYPEID>
class Basic: public Base
{
protected:
    /// The element's value
    ELEMTYPE    m_data;


public:
    /// Constructor
    Basic( ELEMTYPE initialValue = 0,
           bool     inUse        = false,
           bool     validFlag    = false
         );

public:
    /// Updates the data content's value
    virtual void set( ELEMTYPE newValue );

    /// Returns the data content's value
    virtual ELEMTYPE get(void) const;


public: 
    /// See Rte::Element::Api
    void copyDataFrom( const Api& other );

    /// See Rte::Element::Api
    bool isDifferentFrom( const Api& other ) const;

    /// See Rte::Element::Api
    const void* dataPointer( void ) const;

    /// See Rte::Element::Api
    size_t exportElement( void* dstDataStream ) const;

    /// See Rte::Element::Api
    size_t importElement( const void* srcDataStream );

    /// See Rte::Element::Api
    size_t externalSize(void) const;

};

/////////////////////////////////////////////////////////////////////////////
// Pre-defined types to simply element declarations

/// C POD type
typedef Basic<bool,DataType::BOOLEAN>          Boolean_T;

/// C POD type
typedef Basic<int8_t,DataType::INTEGER8>       Integer8_T;

/// C POD type
typedef Basic<uint8_t,DataType::UINTEGER8>     Uinteger8_T;

/// C POD type
typedef Basic<int16_t,DataType::INTEGER16>     Integer16_T;

/// C POD type
typedef Basic<uint16_t,DataType::UINTEGER16>   Uinteger16_T;

/// C POD type
typedef Basic<int32_t,DataType::INTEGER32>     Integer32_T;

/// C POD type
typedef Basic<uint32_t,DataType::UINTEGER32>   Uinteger32_T;

/// C POD type
typedef Basic<int64_t,DataType::INTEGER32>     Integer64_T;

/// C POD type
typedef Basic<uint64_t,DataType::UINTEGER32>   Uinteger64_T;

/// C POD type
typedef Basic<size_t,DataType::SIZE_T>         Size_T;

/// C POD type
typedef Basic<float,DataType::FLOAT>           Float_T;

/// C POD type
typedef Basic<double,DataType::DOUBLE>         Double_T;

/// C POD type
typedef Basic<void*,DataType::VOIDPTR>         VoidPtr_T;


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class ELEMTYPE, int TYPEID>
Rte::Element::Basic<ELEMTYPE,TYPEID>::Basic( ELEMTYPE initialValue,
                                             bool     inUse,
                                             bool     validFlag
                                           )
:Base(TYPEID,inUse,validFlag),
 m_data(initialValue)
    {
    }


/////////////////
template<class ELEMTYPE, int TYPEID>
void Rte::Element::Basic<ELEMTYPE,TYPEID>::set( ELEMTYPE newValue )
    {
    m_data = newValue;
    }

template<class ELEMTYPE, int TYPEID>
ELEMTYPE Rte::Element::Basic<ELEMTYPE,TYPEID>::get( void ) const
    {
    return m_data;
    }

/////////////////
template<class ELEMTYPE, int TYPEID>
void Rte::Element::Basic<ELEMTYPE,TYPEID>::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );
    m_data = *((ELEMTYPE*)(other.dataPointer()));
    }

template<class ELEMTYPE, int TYPEID>
bool Rte::Element::Basic<ELEMTYPE,TYPEID>::isDifferentFrom( const Api& other ) const
    {
    assertTypeMatches( other );
    return m_data != *((ELEMTYPE*)(other.dataPointer()));
    }

template<class ELEMTYPE, int TYPEID>
const void* Rte::Element::Basic<ELEMTYPE,TYPEID>::dataPointer( void ) const
    {
    return &m_data;
    }

/////////////////
template<class ELEMTYPE, int TYPEID>
size_t Rte::Element::Basic<ELEMTYPE,TYPEID>::exportElement( void* dstDataStream ) const
    {
    uint8_t* dstPtr = (uint8_t*)dstDataStream;

    memcpy( dstPtr,                 &m_valid, sizeof(m_valid) );
    memcpy( dstPtr+sizeof(m_valid), &m_data,  sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

template<class ELEMTYPE, int TYPEID>
size_t Rte::Element::Basic<ELEMTYPE,TYPEID>::importElement( const void* srcDataStream )
    {
    const uint8_t* srcPtr = (const uint8_t*)srcDataStream;

    memcpy( &m_valid, srcPtr,                 sizeof(m_valid) );
    memcpy( &m_data,  srcPtr+sizeof(m_valid), sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

template<class ELEMTYPE, int TYPEID>
size_t Rte::Element::Basic<ELEMTYPE,TYPEID>::externalSize( void ) const
    {
    return sizeof(m_data) + sizeof(m_valid);
    }


};      // end namespaces
};
#endif  // end header latch

