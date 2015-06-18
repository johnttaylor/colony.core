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

///
using namespace Rte::Element;


///
#define SECT_   "Rte::Element"


//////////////////////////////////
SeqNumber::SeqNumber( bool inUse, bool validFlag )
:Base(DataType::SEQNUMBER, inUse, validFlag)
,m_data(1) // Default value is 'perform normal copy'
    {
    }

/////////////////
void SeqNumber::set( int32_t newValue )
    {
    m_data = newValue;
    }

int32_t SeqNumber::get( void ) const
    {
    return m_data;
    }


/////////////////
void SeqNumber::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );

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