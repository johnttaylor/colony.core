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

#include "Md5.h"
#include <memory.h>


///
using namespace Rte::Element;


///
#define SECT_   "Rte::Element"

//////////////////////////////////
Md5::Md5( bool inUse, bool validFlag )
:Base(DataType::MD5,inUse,validFlag)
    {
    }

/////////////////
void Md5::set( const Cpl::Checksum::ApiMd5::Digest_T& newValue )
    {
    memcpy(&_data, &newValue, sizeof(m_data));
    }

Cpl::Checksum::ApiMd5::Digest_T& Md5::get( void )
    {
    return m_data;
    }


/////////////////
void Md5::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );
    memcpy(&m_data, other.dataPointer(), sizeof(m_data));
    }

bool Md5::isDifferentFrom( const Api& other ) const
    {
    assertTypeMatches( other );
    return memcmp(m_data, other.dataPointer(), sizeof(m_data)) != 0;
    }

const void* Md5::dataPointer( void ) const
    {
    return &m_data;
    }


/////////////////
size_t Md5::exportElement( void* dstDataStream ) const
    {
    uint8_t* dstPtr = (uint8_t*)dstDataStream;

    memcpy( dstPtr,                 &m_valid, sizeof(m_valid) );
    memcpy( dstPtr+sizeof(m_valid), &m_data,  sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

size_t Md5::importElement( const void* srcDataStream )
    {
    const uint8_t* srcPtr = (const uint8_t*)srcDataStream;

    memcpy( &m_valid, srcPtr,                 sizeof(m_valid) );
    memcpy( &m_data,  srcPtr+sizeof(m_valid), sizeof(m_data) );
    return sizeof(m_data) + sizeof(m_valid);
    }

size_t Md5::externalSize( void ) const
    {
    return sizeof(m_data) + sizeof(m_valid);
    }
