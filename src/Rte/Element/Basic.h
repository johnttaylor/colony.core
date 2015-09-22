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


protected:
    /// Constructor
    Basic( ELEMTYPE initialValue = 0,
           bool     inUse        = false,
           int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
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
class Boolean_T: public Basic<bool,DataType::BOOL> 
{
public:
    /// Constructor
    Boolean_T( bool    initialValue = 0,
               bool    inUse        = false,
               int8_t  validState   = RTE_ELEMENT_API_STATE_INVALID
             );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api.  Format: "T"=true, "F"=false
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
    
};


/// C POD type
class Integer8_T: public Basic<int8_t,DataType::INT8> 
{
public:
    /// int8_t
    Integer8_T( int8_t initialValue = 0,
                bool   inUse        = false,
                int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
              );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};



/// C POD type
class Uinteger8_T: public Basic<uint8_t,DataType::UINT8> 
{
public:
    /// int8_t
    Uinteger8_T( uint8_t initialValue = 0,
                 bool    inUse        = false,
                 int8_t  validState   = RTE_ELEMENT_API_STATE_INVALID
               );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};



/// C POD type
class Integer16_T: public Basic<int16_t,DataType::INT16> 
{
public:
    /// int8_t
    Integer16_T( int16_t initialValue = 0,
                 bool    inUse        = false,
                 int8_t  validState   = RTE_ELEMENT_API_STATE_INVALID
               );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Uinteger16_T: public Basic<uint16_t,DataType::UINT16> 
{
public:
    /// int8_t
    Uinteger16_T( uint16_t initialValue = 0,
                  bool     inUse        = false,
                  int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
                );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Integer32_T: public Basic<int32_t,DataType::INT32> 
{
public:
    /// int8_t
    Integer32_T( int32_t  initialValue = 0,
                 bool     inUse        = false,
                 int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
               );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Uinteger32_T: public Basic<uint32_t,DataType::UINT32> 
{
public:
    /// int8_t
    Uinteger32_T( uint32_t initialValue = 0,
                  bool     inUse        = false,
                  int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
                );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Integer64_T: public Basic<int64_t,DataType::INT64> 
{
public:
    /// int8_t
    Integer64_T( int64_t initialValue = 0,
                 bool     inUse        = false,
                 int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
               );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Uinteger64_T: public Basic<uint64_t,DataType::UINT64> 
{
public:
    /// int8_t
    Uinteger64_T( uint64_t initialValue = 0,
                  bool     inUse        = false,
                  int8_t   validState   = RTE_ELEMENT_API_STATE_INVALID
                );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Size_T: public Basic<size_t,DataType::SIZE_T> 
{
public:
    /// int8_t
    Size_T( size_t initialValue = 0,
            bool   inUse        = false,
            int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
          );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Time_T: public Basic<time_t,DataType::TIME_T> 
{
public:
    /// int8_t
    Time_T( time_t initialValue = 0,
            bool   inUse        = false,
            int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
          );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Float_T: public Basic<float,DataType::FLOAT> 
{
public:
    /// int8_t
    Float_T( float initialValue = 0,
             bool   inUse        = false,
             int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
           );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class Double_T: public Basic<double,DataType::DOUBLE> 
{
public:
    /// int8_t
    Double_T( double initialValue = 0,
              bool   inUse        = false,
              int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
            );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};


/// C POD type
class VoidPtr_T: public Basic<void*,DataType::VOIDPTR> 
{
public:
    /// int8_t
    VoidPtr_T( void*  initialValue = 0,
               bool   inUse        = false,
               int8_t validState   = RTE_ELEMENT_API_STATE_INVALID
             );

public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
};



/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class ELEMTYPE, int TYPEID>
Rte::Element::Basic<ELEMTYPE,TYPEID>::Basic( ELEMTYPE initialValue,
                                             bool     inUse,
                                             int8_t   validState
                                           )
:Base(TYPEID,inUse,validState),
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

