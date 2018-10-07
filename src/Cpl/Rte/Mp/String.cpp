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
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/Frame/StringDecoder.h"
#include "Cpl/Text/Frame/StringEncoder.h"


///
using namespace Cpl::Rte::Mp;

char String::g_buffer[OPTION_CPL_RTE_MP_STRING_MAX_LENGTH_FROM_STRING_BUFFER];

///////////////////////////////////////////////////////////////////////////////
String::String( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, size_t maxLength, const char* initialValue, int8_t validState )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, validState )
    , m_data( maxLength, initialValue )
{
}

///////////////////////////////////////////////////////////////////////////////
uint16_t String::read( Cpl::Text::String& dstData, int8_t& validState ) const throw()
{
    m_modelDatabase.lock_();
    validState = m_validState;
    if ( IS_VALID( validState ) )
    {
        dstData = m_data;
    }
    uint16_t result = m_seqNum;
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
    m_modelDatabase.lock_();
    if ( srcData && testAndUpdateLock( lockRequest ) )
    {
        if ( !IS_VALID( m_validState ) || m_data != srcData )
        {
            m_data.copyIn( srcData, srcLen );
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
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

/// Function 'not supported' for this class.  In theory only called by the Base class write() method which is overloaded for this class
bool String::isDataEqual_( const void* otherData ) const throw()
{
    Cpl::System::FatalError::logf( "Implementation Error!  The Cpl::Rte::Mp::String class does not support the isDataEqual_() method!" );
    return false;
}

/// Function 'not supported' for this class.  In theory only called by the Base class read() method which is overloaded for this class
void String::copyDataTo_( void* dstData, size_t dstSize ) const throw()
{
    Cpl::System::FatalError::logf( "Implementation Error!  The Cpl::Rte::Mp::String class does not support the copyDataTo_() method!" );
}

/// Function 'not supported' for this class.  In theory only called by the Base class write() method which is overloaded for this class
void String::copyDataFrom_( const void* srcData, size_t srcSize ) throw()
{
    Cpl::System::FatalError::logf( "Implementation Error!  The Cpl::Rte::Mp::String class does not support the copyDataFrom_() method!" );
}


///////////////////////////////////////////////////////////////////////////////
const char* String::getTypeAsText() const throw()
{
    return "STRING";
}

size_t String::getSize() const throw()
{
    return m_data.maxLength();
}

void* String::getDataPointer_() throw()
{
    int dummy;
    return m_data.getBuffer( dummy );
}

size_t String::importData( const void* srcDataStream, size_t srcLength, uint16_t* retSeqNum ) throw()
{
    m_modelDatabase.lock_();
    size_t result = ModelPointCommon_::importData( srcDataStream, srcLength, retSeqNum );

    // Ensure that my imported value is null terminated
    if ( result > 0 )
    {
        int   maxLength;
        char* rawPtr      = m_data.getBuffer( maxLength );
        rawPtr[maxLength] = '\0';
    }
    m_modelDatabase.unlock_();

    return result;
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
        encoder.output( m_data.getString() );
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

