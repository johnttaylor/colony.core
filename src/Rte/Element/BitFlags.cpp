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

#include "BitFlags.h"
#include <memory.h>
#include "Cpl/Text/atob.h"


///
using namespace Rte::Element;

///
#define SECT_   "Rte::Element"


//////////////////////////////////
BitFlags::BitFlags( bool inUse, int8_t validState )
:Base(Rte::Element::DataType::BITFLAGS,inUse,validState),
 m_data(0) // Default value is 'perform normal copy'
    {
    }

/////////////////
const char* BitFlags::getTypeAsText(void) const
    {
    return "BITFLAGS";
    }


void BitFlags::set( uint32_t newValue )
    {
    m_data = newValue;
    }

uint32_t BitFlags::get( void ) const
    {
    return m_data;
    }

const char* BitFlags::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%08X", m_data );
        }

    return dstMemory;
    }

const char* BitFlags::fromString( const char* srcText, const char* terminationChars, unsigned* updatedPtr )
    {
    // Cache current state
    unsigned    prevUpdatedValue = updatedPtr? *updatedPtr: 0;
    uint32_t    prevFlags        = m_data;
    int8_t      prevValidState   = validState();

    // Update the sequence number
    const char* result           = Base::fromString( srcText, terminationChars, updatedPtr );

    // Back out the 'set' operation when the requested 'action' was NO_UPDATE -->maintains/enforces the BitFlags Element's semantics!
    if ( result && m_data >= (eOPER_SET_BITS|eOPER_CLR_BITS) )
        {
        m_data = prevFlags;
        setValidState( prevValidState );
        if ( updatedPtr )
            {
            *updatedPtr = prevUpdatedValue;
            }
        }

    return result;
    }
    

const char* BitFlags::setFromText( const char* srcText, const char* terminationChars )
    {
    const char*   endPtr = 0;
    unsigned long temp;

    if ( Cpl::Text::a2ul( temp, srcText, 0, terminationChars, &endPtr ) && temp <= 0xFFFFFFFF && (!isModelElement() || !isLocked()) )
        {
        applyNewValue( (uint32_t) temp );
        return endPtr;
        }
    
    return 0;
    }


/////////////////
bool BitFlags::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );

    // Silent skip when locked AND I am a Model Element
    uint32_t temp = *((uint32_t*)(other.dataPointer()));
    if ( temp < (eOPER_SET_BITS|eOPER_CLR_BITS) )
        {
        if ( !isModelElement() || !isLocked() )
            {
            applyNewValue( *((uint32_t*)(other.dataPointer())) );
            }

        return true;
        }

    return false;
    }

void BitFlags::applyNewValue( uint32_t operAndValue )
    {
    // Normal copy. Note: The caller is not allowed to call this method if 
    // 'operAndValue' >= 0xC0000000.  However to proper handle the fromString()
    // logic/sematic -->it is allowed (okay somewhat of hack!)
    // Normal copy
    if ( operAndValue < eOPER_TGL_BITS )
        {
        m_data = operAndValue;
        }

    // Toggle Bits
    else if ( operAndValue < eOPER_CLR_BITS )
        {
		m_data ^= (operAndValue&eUSABLE_BITS_MASK);
        }

    // Clear bits
    else if ( operAndValue < eOPER_SET_BITS )
        {
        m_data &= ~(operAndValue&eUSABLE_BITS_MASK); 
        }

    // Set bits
    else if ( operAndValue < (eOPER_SET_BITS|eOPER_TGL_BITS) )
        {
        m_data |= (operAndValue&eUSABLE_BITS_MASK); 
        }

    // SPECIAL Case: The caller is not allowed to call this method if 
    // 'operAndValue' >= 0xC0000000.  However to proper handle the fromString()
    // logic/sematic -->it is allowed (okay somewhat of hack!)
    else
        {
        m_data = operAndValue;
        }
    }

bool BitFlags::isDifferentFrom( const Api& other ) const
    {
    assertTypeMatches( other );
    return m_data != *((uint32_t*)(other.dataPointer()));
    }

const void* BitFlags::dataPointer( void ) const
    {
    return &m_data;
    }


/////////////////
size_t BitFlags::exportElement( void* dstDataStream ) const
    {
    uint8_t* dstPtr = (uint8_t*)dstDataStream;

    memcpy( dstPtr,                 &m_valid, sizeof(m_valid) );
    memcpy( dstPtr+sizeof(m_valid), &m_data,  sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

size_t BitFlags::importElement( const void* srcDataStream )
    {
    const uint8_t* srcPtr = (const uint8_t*)srcDataStream;

    memcpy( &m_valid, srcPtr,                 sizeof(m_valid) );
    memcpy( &m_data,  srcPtr+sizeof(m_valid), sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

size_t BitFlags::externalSize( void ) const
    {
    return sizeof(m_data) + sizeof(m_valid);
    }



