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

#include "Bitflags.h"
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
    dstMemory.formatOpt( append, "%08X", m_data );
    return dstMemory;
    }

const char* BitFlags::setFromText( const char* srcText, const char* terminationChars )
    {
    const char*   endPtr = 0;
    unsigned long temp;

    if ( Cpl::Text::a2ul( temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp <= eUSABLE_BITS_MASK )
            {
            m_data = temp;
            return endPtr;
            }
        }
    
    return 0;
    }


/////////////////
bool BitFlags::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );

    // Silent skip when locked AND I am a Model Element
    if ( !isModelElement() || !isLocked() )
        {
        uint32_t src = *((uint32_t*)(other.dataPointer()));

        // Normal copy
        if ( src < eOPER_TGL_BITS )
            {
            m_data = src;
            }

        // Toggle Bits
        else if ( src < eOPER_CLR_BITS )
            {
            m_data ^= src;
            }

        // Clear bits
        else if ( src < eOPER_SET_BITS )
            {
            m_data &= ~(src&eUSABLE_BITS_MASK); 
            }

        // Set bits
        else if ( src < (eOPER_SET_BITS|eOPER_TGL_BITS) )
            {
            m_data |= (src&eUSABLE_BITS_MASK); 
            }

        return true;
        }

    return false;
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
