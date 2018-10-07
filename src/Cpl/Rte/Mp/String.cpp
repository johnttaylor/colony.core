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


#include "String.h"
#include "Cpl/System/Assert.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/Frame/StringDecoder.h"
#include "Cpl/Text/Frame/StringEncoder.h"
#include "Cpl/Memory/Allocator.h"
#include <string.h>


///
using namespace Cpl::Rte::Mp;

char String::g_buffer[OPTION_CPL_RTE_MP_STRING_MAX_LENGTH_FROM_STRING_BUFFER];

static char emptyString_[1] = { '\0' };

///////////////////////////////////////////////////////////////////////////////
String::String( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, size_t maxLength, const char* initialValue, int8_t validState )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, validState )
    , m_data( new(std::nothrow) char[maxLength + 1] )
    , m_maxLength( maxLength )
{
    // Trapped failed to allocate memory -->silent fail and set string size to zero
    if ( m_data == 0 )
    {
        m_data      = emptyString_;
        m_maxLength = 0;
    }

    // Set the initial value
    if ( initialValue )
    {
        strncpy( m_data, initialValue, maxLength );
        m_data[maxLength] = '\0';
    }

    // Null pointer for initial value -->set initial value to an empty string
    else
    {
        m_data[0] = '\0';
    }
}

///////////////////////////////////////////////////////////////////////////////
uint16_t String::read( Cpl::Text::String& dstData, int8_t& validState ) const throw()
{
    m_modelDatabase.lock_();
    int      dstLength;
    char*    dstPtr = dstData.getBuffer( dstLength );
    uint16_t result =  ModelPointCommon_::read( dstPtr, dstLength+1, validState );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t String::write( const char* newValue, LockRequest_T lockRequest ) throw()
{
    // Trap the null pointer case -->Do NOTHING
    if ( newValue == 0 )
    {
        m_modelDatabase.lock_();
        uint16_t seqNum = m_seqNum;
        m_modelDatabase.unlock_();
        return seqNum;
    }

    return write( newValue, strlen( newValue ), lockRequest );
}

uint16_t String::write( const char* srcData, size_t srcLen, LockRequest_T lockRequest = eNO_REQUEST ) throw()
{
    return ModelPointCommon_::write( srcData, srcLen+1, lockRequest );
}

uint16_t String::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void String::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void String::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}

bool String::isDataEqual_( const void* otherData ) const throw()
{
    size_t otherLen = strlen( (char*) otherData );
    size_t myLen    = strlen( m_data );
    return otherLen == myLen && strcmp( m_data, (char*) otherData ) == 0;
}

void String::copyDataTo_( void* dstData, size_t dstSize ) const throw()
{
    CPL_SYSTEM_ASSERT( dstSize >= m_maxLength + 1 );
    memcpy( dstData, m_data, m_maxLength + 1 );
}

void String::copyDataFrom_( const void* srcData, size_t srcSize ) throw()
{
    CPL_SYSTEM_ASSERT( srcSize <= m_maxLength + 1 );
    memcpy( m_data, srcData, srcSize );
}


///////////////////////////////////////////////////////////////////////////////
const char* String::getTypeAsText() const throw()
{
    return "STRING";
}

size_t String::getSize() const throw()
{
    return m_maxLength + 1;
}

size_t String::getInternalSize_() const throw()
{
    return m_maxLength + 1;
}


void* String::getDataPointer_() throw()
{
    return m_data;
}

bool String::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const throw()
{
    // Lock the database
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;

    // Convert data and state to a string
    if ( convertStateToText( dst, append, m_locked, m_validState ) )
    {
        // Output as a "Text String"
        Cpl::Text::Frame::StringEncoder encoder( dst, OPTION_CPL_RTE_MODEL_POINT_QUOTE_CHAR, OPTION_CPL_RTE_MODEL_POINT_QUOTE_CHAR, OPTION_CPL_RTE_MODEL_POINT_ESCAPE_CHAR, false, append );
        encoder.startFrame();
        encoder.output( m_data );
        encoder.endFrame();
    }

    // Unlock the database
    m_modelDatabase.unlock_();

    // Return the sequence if/when requested
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    return true;
}

const char* String::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    const char*   result = 0;
    uint16_t      seqnum = SEQUENCE_NUMBER_UNKNOWN;
    const char*   endPtr;

    // Allow non-quoted text (the burden is on the caller that the text does 
    // NOT contain any 'special' characters where this would get us in 
    // trouble).  This is done as CONVENIENCE for the use case of a human typing
    // in the srcText 
    if ( *srcText != OPTION_CPL_RTE_MODEL_POINT_QUOTE_CHAR )
    {
        // At least one termination character specified
        if ( terminationChars && *terminationChars != '\0' )
        {
            endPtr = Cpl::Text::stripNotChars( srcText, terminationChars );
        }

        // No terminator characters -->consume ALL characters
        else
        {
            endPtr = srcText + strlen( srcText );
        }

        // Update the model point
        seqnum = write( srcText, endPtr - srcText, lockAction );
        result = endPtr;
    }

    // Quoted text use case (aka a "Text String")
    else
    {
        Cpl::Text::Frame::StringDecoder decoder( OPTION_CPL_RTE_MODEL_POINT_QUOTE_CHAR, OPTION_CPL_RTE_MODEL_POINT_QUOTE_CHAR, OPTION_CPL_RTE_MODEL_POINT_ESCAPE_CHAR, srcText );
        size_t                          decodedSize = 0;

        // Lock the database since I have ONLY ONE instance of g_buffer
        m_modelDatabase.lock_();
        if ( decoder.scan( OPTION_CPL_RTE_MP_STRING_MAX_LENGTH_FROM_STRING_BUFFER, g_buffer, decodedSize ) )
        {
            // Update the Model Point
            seqnum = write( g_buffer, decodedSize, lockAction );
            endPtr = decoder.getRemainder();
        }
        m_modelDatabase.unlock_();
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

