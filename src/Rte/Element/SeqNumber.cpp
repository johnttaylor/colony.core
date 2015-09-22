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

#include "SeqNumber.h"
#include <memory.h>
#include "Cpl/Text/atob.h"


///
using namespace Rte::Element;


///
#define SECT_   "Rte::Element"


//////////////////////////////////
SeqNumber::SeqNumber( bool inUse, int8_t validState )
:Base(DataType::SEQNUM, inUse, validState)
,m_data(1) // Default value is 'perform normal copy'
    {
    }

/////////////////
const char* SeqNumber::getTypeAsText(void) const
    {
    return "SEQNUM";
    }


void SeqNumber::set( int32_t newValue )
    {
    m_data = newValue;
    }

int32_t SeqNumber::get( void ) const
    {
    return m_data;
    }


const char* SeqNumber::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%lu", m_data );
        }

    return dstMemory;
    }

const char* SeqNumber::setFromText( const char* srcText, const char* terminationChars )
    {
    {
    const char* endPtr = 0;
    long        temp;

    if ( Cpl::Text::a2l( temp, srcText, 0, terminationChars, &endPtr ) )
        {
        // Sequence numbers (in the Model) can NOT be negative OR zero
        if ( (temp ^ 0x7FFFFFFF) == 0 )
            {
            m_data = (int32_t) temp;
            return endPtr;
            }
        }
    
    return 0;
    }
    }

/////////////////
bool SeqNumber::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );

    // Silently ignore when locked AND I am a Model Element
    if ( !isModelElement() || !isLocked() )
        {
        int32_t src = *((int32_t*)(other.dataPointer()));
        if ( src < 0 )
            {
            // When incrementing - don't let the value go negative 
            if ( ++m_data <= 0 )
                {
                m_data = 1; // By deisgn, a valid sequence number is never zero
                }
            }
        else if ( src > 0 )
            {
            m_data = src;
            }

        return true;
        }

    return false;
    }



bool SeqNumber::isDifferentFrom( const Api& other ) const
    {
    assertTypeMatches( other );
    return m_data != *((int32_t*)(other.dataPointer()));
    }

const void* SeqNumber::dataPointer( void ) const
    {
    return &m_data;
    }

/////////////////
size_t SeqNumber::exportElement( void* dstDataStream ) const
    {
    uint8_t* dstPtr = (uint8_t*)dstDataStream;

    memcpy( dstPtr,                 &m_valid, sizeof(m_valid) );
    memcpy( dstPtr+sizeof(m_valid), &m_data,  sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

size_t SeqNumber::importElement( const void* srcDataStream )
    {
    const uint8_t* srcPtr = (const uint8_t*)srcDataStream;

    memcpy( &m_valid, srcPtr,                 sizeof(m_valid) );
    memcpy( &m_data,  srcPtr+sizeof(m_valid), sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

size_t SeqNumber::externalSize( void ) const
    {
    return sizeof(m_data) + sizeof(m_valid);
    }
