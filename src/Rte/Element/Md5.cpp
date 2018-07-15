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

#include "Md5.h"
#include <memory.h>
#include "Cpl/Text/FString.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/atob.h"


///
using namespace Rte::Element;


///
#define SECT_   "Rte::Element"

//////////////////////////////////
Md5::Md5( bool inUse, int8_t validState )
:Base(DataType::MD5,inUse,validState)
    {
    }

/////////////////
const char* Md5::getTypeAsText(void) const
    {
    return "MD5";
    }

void Md5::set( const Cpl::Checksum::ApiMd5::Digest_T& newValue )
    {
    memcpy(&m_data, &newValue, sizeof(m_data));
    }

Cpl::Checksum::ApiMd5::Digest_T& Md5::get( void )
    {
    return m_data;
    }


const char* Md5::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        Cpl::Text::bufferToAsciiHex( m_data, sizeof(m_data), dstMemory, true, append );
        }

    return dstMemory;
    }


const char* Md5::setFromText( const char* srcText, const char* terminationChars )
    {
    Cpl::Text::FString<sizeof(m_data)*2> temp;
    const char*                          endPtr = Cpl::Text::stripNotChars( srcText, terminationChars );
	size_t                               len    = endPtr - srcText;
    temp.copyIn( srcText, len );
    temp += "00000000000000000000000000000000";

    if ( !isModelElement() || !isLocked() )
        {
        if ( !Cpl::Text::asciiHexToBuffer( m_data, temp, sizeof(m_data)*2) )
            {
            endPtr = 0;
            }
        }

    return endPtr;
    }

/////////////////
bool Md5::copyDataFrom( const Api& other )
    {
    assertTypeMatches( other );
    if ( !isModelElement() || !isLocked() )
        {
        memcpy(&m_data, other.dataPointer(), sizeof(m_data));
        return true;
        }

    return false;
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
