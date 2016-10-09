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
#include <time.h>

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


protected:
    /// Constructor
    Basic( ELEMTYPE initialValue    = (ELEMTYPE)0,
           bool     inUse           = false,
           int8_t   validState      = RTE_ELEMENT_API_STATE_INVALID,
           int      specializedType = TYPEID
         );

public:
    /// Updates the data content's value
    virtual void set( ELEMTYPE newValue );

    /// Returns the data content's value
    virtual ELEMTYPE get(void) const;


public: 
    /// See Rte::Element::Api
    bool copyDataFrom( const Api& other );

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

/// C POD type
class Boolean: public Basic<bool,DataType::BOOL> 
{
public:
    /// Constructor
    Boolean( bool    initialValue = 0,
             bool    inUse        = false,
             int8_t  validState   = RTE_ELEMENT_API_STATE_INVALID
           );

protected:
    /// Constructor
    Boolean( int     specializedType,
             bool    initialValue = 0,
             bool    inUse        = false,
             int8_t  validState   = RTE_ELEMENT_API_STATE_INVALID
           );


public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api.  Format: "T"=true, "F"=false
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
    
};


/// C POD type
class Integer8: public Basic<int8_t,DataType::INT8> 
{
public:
    /// int8_t
    Integer8( int8_t initialValue = 0,
              bool   inUse        = false,
              int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
            );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};



/// C POD type
class Uinteger8: public Basic<uint8_t,DataType::UINT8> 
{
public:
    /// int8_t
    Uinteger8( uint8_t initialValue = 0,
               bool    inUse        = false,
               int8_t  validState   = RTE_ELEMENT_API_STATE_INVALID
             );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};



/// C POD type
class Integer16: public Basic<int16_t,DataType::INT16> 
{
public:
    /// int8_t
    Integer16( int16_t initialValue = 0,
               bool    inUse        = false,
               int8_t  validState   = RTE_ELEMENT_API_STATE_INVALID
             );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Uinteger16: public Basic<uint16_t,DataType::UINT16> 
{
public:
    /// int8_t
    Uinteger16( uint16_t initialValue = 0,
                bool     inUse        = false,
                int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
              );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Integer32: public Basic<int32_t,DataType::INT32> 
{
public:
    /// int8_t
    Integer32( int32_t  initialValue = 0,
               bool     inUse        = false,
               int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
             );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Uinteger32: public Basic<uint32_t,DataType::UINT32> 
{
public:
    /// int8_t
    Uinteger32( uint32_t initialValue = 0,
                bool     inUse        = false,
                int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
              );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Integer64: public Basic<int64_t,DataType::INT64> 
{
public:
    /// int8_t
    Integer64( int64_t initialValue = 0,
               bool     inUse        = false,
               int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
             );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Uinteger64: public Basic<uint64_t,DataType::UINT64> 
{
public:
    /// int8_t
    Uinteger64( uint64_t initialValue = 0,
                bool     inUse        = false,
                int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
              );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class SizeT: public Basic<size_t,DataType::SIZE_T> 
{
public:
    /// int8_t
    SizeT( size_t initialValue = 0,
           bool   inUse        = false,
           int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
         );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class TimeT: public Basic<time_t,DataType::TIME_T> 
{
public:
    /// int8_t
    TimeT( time_t initialValue = 0,
           bool   inUse        = false,
           int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
         );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Float: public Basic<float,DataType::FLOAT> 
{
public:
    /// int8_t
    Float( float initialValue = 0,
           bool   inUse        = false,
           int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
         );

public:
    /// See Rte::Element::Api (need to use "almost equal" since this is floating point)
    bool isDifferentFrom( const Api& other ) const;

    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Double: public Basic<double,DataType::DOUBLE> 
{
public:
    /// int8_t
    Double( double initialValue = 0,
            bool   inUse        = false,
            int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
          );

public:
    /// See Rte::Element::Api (need to use "almost equal" since this is floating point)
    bool isDifferentFrom( const Api& other ) const;

    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class VoidPtr: public Basic<void*,DataType::VOIDPTR> 
{
public:
    /// int8_t
    VoidPtr( void*  initialValue = 0,
             bool   inUse        = false,
             int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
           );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};



/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class ELEMTYPE, int TYPEID>
Rte::Element::Basic<ELEMTYPE,TYPEID>::Basic( ELEMTYPE initialValue,
                                             bool     inUse,
                                             int8_t   validState,
           			    	     int      specializedType
					   )
:Base(specializedType,inUse,validState),
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
bool Rte::Element::Basic<ELEMTYPE,TYPEID>::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );
    if ( !isModelElement() || !isLocked() )
        {
        m_data = *((ELEMTYPE*)(other.dataPointer()));
        return true;
        }

    return false;
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

