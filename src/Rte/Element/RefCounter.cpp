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

#include "RefCounter.h"
#include <memory.h>
#include "Cpl/Text/atob.h"


///
using namespace Rte::Element;


///
#define SECT_   "Rte::Element"


//////////////////////////////////
RefCounter::RefCounter( bool inUse, int8_t validState )
:Base(DataType::REFCOUNT, inUse, validState)
,m_data(0) // Default value is start the reference count at zero
    {
    }

/////////////////
const char* RefCounter::getTypeAsText(void) const
    {
    return "REFCOUNT";
    }


void RefCounter::set( int32_t newValue )
    {
    m_data = newValue;
    }

int32_t RefCounter::get( void ) const
    {
    return m_data;
    }


const char* RefCounter::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%ld", m_data );
        }

    return dstMemory;
    }


const char* RefCounter::fromString( const char* srcText, const char* terminationChars, unsigned* updatedPtr )
    {
    // Cache current state
    unsigned    prevUpdatedValue = updatedPtr? *updatedPtr: 0;
    int32_t     prevRefCnt       = m_data;
    int8_t      prevValidState   = validState();

    // Update the sequence number
    const char* result           = Base::fromString( srcText, terminationChars, updatedPtr );

    // Back out the 'set' operation when the requested refence counter value was eNOP (-3) -->maintains/enforces the RefCounter Element's semantics!
    if ( result && m_data == eNOP )
        {
        m_data = prevRefCnt;
        setValidState( prevValidState );
        if ( updatedPtr )
            {
            *updatedPtr = prevUpdatedValue;
            }
        }

    return result;
    }
    

const char* RefCounter::setFromText( const char* srcText, const char* terminationChars )
    {
    {
    const char* endPtr = 0;
    long        temp;

    if ( Cpl::Text::a2l( temp, srcText, 0, terminationChars, &endPtr ) && temp <= INT32_MAX && temp >= INT32_MIN )
        {
        if ( !isModelElement() || !isLocked() )
            {
            applyNewValue( (int32_t) temp );
            }

        return endPtr;
        }
    
    return 0;
    }
    }

/////////////////
bool RefCounter::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );

    // Silently ignore when locked AND I am a Model Element (also ignore a value of zero per the RefCounter semantics)
    int32_t temp = *((int32_t*)(other.dataPointer()));
    if ( temp != eNOP && (!isModelElement() || !isLocked()) )
        {
        applyNewValue( temp );
        return true;
        }

    return false;
    }


void RefCounter::applyNewValue( int32_t operAndValue )
    {
    // INCREMENT the reference counter
    if ( operAndValue == eINCREMENT )
        {
        // don't let the value go negative 
        if ( ++m_data < 0 )
            {
            m_data--;
            }
        }

    // INCREMENT the reference counter
    else if ( operAndValue == eDECREMENT )
        {
        // don't let the value go negative 
        if ( m_data > 0 )
            {
            m_data--;
            }
        }


    // SET the Reference counter (Note: The caller is not allowed to call this method if 'operAndValue' is eNOP )
    else 
        {
        m_data = operAndValue;
        }
    }


bool RefCounter::isDifferentFrom( const Api& other ) const
    {
    assertTypeMatches( other );
    return m_data != *((int32_t*)(other.dataPointer()));
    }

const void* RefCounter::dataPointer( void ) const
    {
    return &m_data;
    }

/////////////////
size_t RefCounter::exportElement( void* dstDataStream ) const
    {
    uint8_t* dstPtr = (uint8_t*)dstDataStream;

    memcpy( dstPtr,                 &m_valid, sizeof(m_valid) );
    memcpy( dstPtr+sizeof(m_valid), &m_data,  sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

size_t RefCounter::importElement( const void* srcDataStream )
    {
    const uint8_t* srcPtr = (const uint8_t*)srcDataStream;

    memcpy( &m_valid, srcPtr,                 sizeof(m_valid) );
    memcpy( &m_data,  srcPtr+sizeof(m_valid), sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

size_t RefCounter::externalSize( void ) const
    {
    return sizeof(m_data) + sizeof(m_valid);
    }
