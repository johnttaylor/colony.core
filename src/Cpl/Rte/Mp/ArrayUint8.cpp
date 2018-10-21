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
/** @file */


#include "ArrayUint8.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/atob.h"


///
using namespace Cpl::Rte::Mp;


///////////////////////////////////////////////////////////////////////////////
ArrayUint8::ArrayUint8( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, bool decimalFormat, int8_t validState, uint8_t* srcData )
    :Array<uint8_t>( myModelBase, staticInfo, numElements, validState, srcData )
    , m_decimal( decimalFormat )
{
}


///////////////////////////////////////////////////////////////////////////////
uint16_t ArrayUint8::read( uint8_t* dstData, size_t dstNumElements, int8_t& validState, size_t srcIndex ) const throw()
{
    InternalData dst = { dstData, dstNumElements, srcIndex };
    return ModelPointCommon_::read( &dst, sizeof( dst ), validState );
}

uint16_t ArrayUint8::write( uint8_t* srcData, size_t srcNumElements, LockRequest_T lockRequest, size_t dstIndex ) throw()
{
    InternalData src = { srcData, srcNumElements, dstIndex };
    return ModelPointCommon_::write( &src, sizeof( src ), lockRequest );
}

uint16_t ArrayUint8::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}


///////////////////////////////////////////////////////////////////////////////
void ArrayUint8::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void ArrayUint8::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* ArrayUint8::getTypeAsText() const throw()
{
    return m_decimal ? "Cpl::Rte::Mp::ArrayUint8-dec" : "Cpl::Rte::Mp::ArrayUint8-hex";
}

bool ArrayUint8::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const throw()
{
    // lock the database
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;

    // Convert data and state to a string
    if ( convertStateToText( dst, append, m_locked, m_validState ) )
    {
        // Output the element count and starting index
        dst.formatOpt( append, "%lu:0:", (unsigned long) m_data.numElements );

        // DECIMAL format
        if ( m_decimal )
        {
            // Output the elements
            size_t idx;
            for ( idx=0; idx < m_data.numElements; idx++ )
            {
                if ( idx == 0 )
                {
                    dst.formatOpt( true, "%d", m_data.elemPtr[idx] );
                }
                else
                {
                    dst.formatOpt( true, ",%d", m_data.elemPtr[idx] );
                }
            }
        }

        // HEX format
        else
        {
            Cpl::Text::bufferToAsciiHex( m_data.elemPtr, m_data.numElements /* * sizeof(uint8_t) */, dst, true, true );
        }
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    // unlock the database
    m_modelDatabase.unlock_();

    return true;
}

const char* ArrayUint8::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    // Lock the database 
    m_modelDatabase.lock_();

    // Default the returned sequence number to the current value (before any update)
    if ( retSequenceNumber )
    {
        *retSequenceNumber = m_seqNum;
    }

    // Parse numElements and starting index
    const char*   endptr;
    unsigned long numElements;
    if ( Cpl::Text::a2ul( numElements, srcText, 10, ":", &endptr ) == false )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Unable to parse the numElems field.  Format is: <numElems>:<mpIndex>:<e0> [%s]", srcText );
        }
        return 0;
    }
    const char*   endptr2;
    unsigned long startIndex;
    if ( Cpl::Text::a2ul( startIndex, endptr, 10, ":", &endptr2 ) == false )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Unable to parse the mpIndex field.  Format is: <numElems>:<mpIndex>:<e0> [%s]", srcText );
        }
        return 0;
    }
    if ( numElements + startIndex > m_data.numElements )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Unable to parse the numElems+mpIndex exceeds the size of the array (array size=%lu) [%s]", (unsigned long) m_data.numElements, srcText );
        }
        return 0;
    }

    // DECIMAL parse
    const char* dataPtr = endptr2;
    if ( m_decimal )
    {
        unsigned long elemNum = 0;
        while ( startIndex < m_data.numElements && *dataPtr != '\0' && strchr( terminationChars, *dataPtr ) != 0 )
        {
            // Parse the next element
            unsigned elemValue;
            if ( Cpl::Text::a2ui( elemValue, dataPtr, 10, ",", &endptr ) == false )
            {
                if ( errorMsg )
                {
                    errorMsg->format( "Failed to parse element number %lu.  Note: A partial write can/did occurred [%s]", elemNum, srcText );
                }
                break;
            }

            // Update the Model Point's data
            else
            {
                m_data.elemPtr[startIndex] = (uint8_t) elemValue;
                dataPtr                    = endptr;
                startIndex++;
                elemNum++;
            }

        }
    }

    // HEX Parse
    else
    {
    }
    Cpl::Text::Frame::StringDecoder decoder( OPTION_CPL_RTE_MODEL_POINT_QUOTE_CHAR, OPTION_CPL_RTE_MODEL_POINT_QUOTE_CHAR, OPTION_CPL_RTE_MODEL_POINT_ESCAPE_CHAR, srcText );
    size_t                          decodedSize = 0;

    if ( decoder.scan( OPTION_CPL_RTE_MP_STRING_MAX_LENGTH_FROM_STRING_BUFFER, g_buffer, decodedSize ) )
    {
        // Update the Model Point
        seqnum = write( g_buffer, decodedSize, lockAction );
        result = decoder.getRemainder();
    }
    else
    {
        if ( errorMsg )
        {
            errorMsg->format( "Conversion of %s as a \"Text_String\"[%s] to a string failed.", getTypeAsText(), srcText );
        }
    }
    m_modelDatabase.unlock_();

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}
