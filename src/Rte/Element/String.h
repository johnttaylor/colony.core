#ifndef Rte_Element_String_h_
#define Rte_Element_String_h_
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
#include "Cpl/Text/FString.h"
#include <stdint.h>


///
namespace Rte { namespace Element {

/** This template class provides a concrete implemenation for a Element
    that has data content of Cpl::Text::FString.  The template parameter
    is the size of the FString
 */

template<int S>
class String: public Base
{
protected:
    /// Storage for the string
    Cpl::Text::FString<S>   m_data;


public:
    /// Constructor
    String( const char* initialValue = "",
            bool        inUse        = false,
            int8_t      validState   = RTE_ELEMENT_API_STATE_INVALID
          );


protected:
    /// Restricted Constructor
    String( int         specializeType,
            const char* initialValue = "",
            bool        inUse        = false,
            int8_t      validState   = RTE_ELEMENT_API_STATE_INVALID
          );

public:
    /// Updates the data content's value
    virtual void set( const char* newValue );

    /// Returns the data content's value
    virtual const char* get(void) const;

    /// Returns a reference to the actual Cpl::Text String object
    virtual Cpl::Text::String& getString(void);


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
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<int S>
Rte::Element::String<S>::String( const char*  initialValue,
                                 bool         inUse,
                                 int8_t       validState
                               )
:Base(Rte::Element::DataType::STRING,inUse,validState),
 m_data(initialValue)
    {
    }

template<int S>
Rte::Element::String<S>::String( int          specializeType,
                                 const char*  initialValue,
                                 bool         inUse,
                                 int8_t       validState
                               )
:Base(specializeType,inUse,validState),
 m_data(initialValue)
    {
    }

/////////////////
template<int S>
void Rte::Element::String<S>::set( const char* newValue )
    {
    m_data = newValue;
    }

template<int S>
const char* Rte::Element::String<S>::get( void ) const
    {
    return m_data;
    }

template<int S>
Cpl::Text::String& Rte::Element::String<S>::getString( void )
    {
    return m_data;
    }


/////////////////
template<int S>
bool Rte::Element::String<S>::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );
    if ( !isModelElement() || !isLocked() )
        {
        m_data = *((Cpl::Text::String*)(other.dataPointer()));
        return true;
        }

    return false;
    }

template<int S>
bool Rte::Element::String<S>::isDifferentFrom( const Api& other ) const
    {
    assertTypeMatches( other );
    return m_data != *((Cpl::Text::String*)(other.dataPointer()));
    }

template<int S>
const void* Rte::Element::String<S>::dataPointer( void ) const
    {
    return (Cpl::Text::String*)(&m_data);
    }


/////////////////
template<int S>
size_t Rte::Element::String<S>::exportElement( void* dstDataStream ) const
    {
    uint8_t* dstPtr = (uint8_t*)dstDataStream;

    memcpy( dstPtr,                 &m_valid, sizeof(m_valid) );
    memcpy( dstPtr+sizeof(m_valid), m_data(), S );
    return S + sizeof(m_valid);
    }

template<int S>
size_t Rte::Element::String<S>::importElement( const void* srcDataStream )
    {
    int            dummy;
    char*          dstPtr = (char*) m_data.getBuffer(dummy);
    const uint8_t* srcPtr = (const uint8_t*) srcDataStream;

    memcpy( &m_valid, srcPtr,                 sizeof(m_valid) );
    memcpy( dstPtr,   srcPtr+sizeof(m_valid), S );
    dstPtr[S] = '\0';
    return S + sizeof(m_valid);
    }

template<int S>
size_t Rte::Element::String<S>::externalSize( void ) const
    {
    return S + sizeof(m_valid);
    }


};      // end namespaces
};
#endif  // end header latch

