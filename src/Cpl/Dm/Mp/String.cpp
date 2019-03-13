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
/** @file */


#include "String.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/Frame/StringDecoder.h"
#include "Cpl/Text/Frame/StringEncoder.h"
#include <string.h>


///
using namespace Cpl::Dm::Mp;

static char emptyString_[1] = { '\0' };

///////////////////////////////////////////////////////////////////////////////
String::String( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, size_t maxLength )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID )
    , m_data( { new(std::nothrow) char[maxLength + 1], 0, maxLength } )
{
    // Throw a fatal error if global parse buffer is too small
    if ( OPTION_CPL_RTE_MODEL_DATABASE_MAX_LENGTH_FROM_STRING_BUFFER < maxLength )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::String().  Creating a string of size %lu which is greater than the fromString() parser buffer", maxLength );
    }

    // Trapped failed to allocate memory -->silent fail and set string size to zero
    if ( m_data.stringPtr == 0 )
    {
        m_data.stringPtr = emptyString_;
        m_data.maxLength = 0;
    }
    else
    {
        // For deterministic value - initialize data to 'empty string'Null pointer for initial value -->set initial value to an empty string
        m_data.stringPtr[0] = '\0';
        m_data.stringLen    = 0;
    }
}

String::String( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, size_t maxLength, const char* initialValue )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, Cpl::Dm::ModelPoint::MODEL_POINT_STATE_VALID )
    , m_data( { new(std::nothrow) char[maxLength + 1], 0, maxLength } )
{
    // Throw a fatal error if global parse buffer is too small
    if ( OPTION_CPL_RTE_MODEL_DATABASE_MAX_LENGTH_FROM_STRING_BUFFER < maxLength )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::String().  Creating a string of size %lu which is greater than the fromString() parser buffer", maxLength );
    }

    // Trapped failed to allocate memory -->silent fail and set string size to zero
    if ( m_data.stringPtr == 0 )
    {
        m_data.stringPtr = emptyString_;
        m_data.maxLength = 0;
    }

    // Set the initial value
    if ( initialValue )
    {
        strncpy( m_data.stringPtr, initialValue, maxLength );
        m_data.stringPtr[m_data.maxLength] = '\0';
    }

    // Null pointer for initial value -->set initial value to an empty string
    else
    {
        m_data.stringPtr[0] = '\0';
    }

    // Set my string length
    m_data.stringLen = strlen( m_data.stringPtr );
}

String::~String()
{
    // Free up the memory IF it was allocated from the heap
    if ( m_data.maxLength > 0 )
    {
        delete m_data.stringPtr;
    }
}

///////////////////////////////////////////////////////////////////////////////
int8_t String::read( Cpl::Text::String& dstData, uint16_t* seqNumPtr ) const throw()
{
    Data dst;
    int  bufferMaxLength;
    dst.stringPtr   = dstData.getBuffer( bufferMaxLength );
    dst.maxLength   = bufferMaxLength;
    return ModelPointCommon_::read( &dst, sizeof( Data ), seqNumPtr );
}

int8_t String::read( Data& dstData, uint16_t* seqNumPtr ) const throw()
{
    return ModelPointCommon_::read( &dstData, sizeof( Data ), seqNumPtr );
}


uint16_t String::write( const Data& srcData, LockRequest_T lockRequest ) throw()
{
    // Trap the null pointer case -->Do NOTHING
    if ( srcData.stringPtr == 0 )
    {
        m_modelDatabase.lock_();
        uint16_t seqNum = m_seqNum;
        m_modelDatabase.unlock_();
        return seqNum;
    }

    return write( srcData.stringPtr, strlen( srcData.stringPtr ), lockRequest );
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

uint16_t String::write( const char* srcData, size_t srcLen, LockRequest_T lockRequest ) throw()
{
    Data src = { (char*) srcData, srcLen, srcLen };
    return ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );
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
    Data*  otherDataPtr = (Data*) otherData;

    return otherDataPtr->stringLen == m_data.stringLen && strncmp( m_data.stringPtr, otherDataPtr->stringPtr, m_data.stringLen ) == 0;
}

void String::copyDataTo_( void* dstData, size_t dstSize ) const throw()
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Data ) );
    Data* dataDstPtr = (Data*) dstData;

    dstSize = dataDstPtr->maxLength >= m_data.stringLen ? m_data.stringLen : dataDstPtr->maxLength;
    memcpy( dataDstPtr->stringPtr, m_data.stringPtr, dstSize );
    dataDstPtr->stringPtr[dstSize] = '\0';
    dataDstPtr->stringLen          = dstSize;
}

void String::copyDataFrom_( const void* srcData, size_t srcSize ) throw()
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Data ) );
    Data* dataSrcPtr = (Data*) srcData;

    srcSize = dataSrcPtr->stringLen <= m_data.maxLength ? dataSrcPtr->stringLen : m_data.maxLength;
    memcpy( m_data.stringPtr, dataSrcPtr->stringPtr, srcSize );
    m_data.stringPtr[srcSize] = '\0';
    m_data.stringLen          = srcSize;
}


///////////////////////////////////////////////////////////////////////////////
const char* String::getTypeAsText() const throw()
{
    return "Cpl::Dm::Mp::String";
}

size_t String::getSize() const throw()
{
    return m_data.maxLength;
}

size_t String::getInternalDataSize() const throw()
{
    return m_data.maxLength;
}


const void* String::getImportExportDataPointer_() const throw()
{
    return m_data.stringPtr;
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
        encoder.output( m_data.stringPtr );
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
    const char*   endPtr;
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;
    m_modelDatabase.unlock_();

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

        // Lock the database since there is ONLY ONE instance of g_buffer
        ModelDatabase::globalLock_();
        if ( decoder.scan( OPTION_CPL_RTE_MODEL_DATABASE_MAX_LENGTH_FROM_STRING_BUFFER, ModelDatabase::g_parseBuffer_, decodedSize ) )
        {
            // Update the Model Point
            seqnum = write( ModelDatabase::g_parseBuffer_, decodedSize, lockAction );
            result = decoder.getRemainder();
        }
        else
        {
            if ( errorMsg )
            {
                errorMsg->format( "Conversion of %s as a \"Text_String\"[%s] to a string failed.", getTypeAsText(), srcText );
            }
        }
        ModelDatabase::globalUnlock_();
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

