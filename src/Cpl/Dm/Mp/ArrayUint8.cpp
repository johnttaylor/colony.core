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


#include "ArrayUint8.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/atob.h"
#include <stdarg.h>


///
using namespace Cpl::Dm::Mp;


///////////////////////////////////////////////////////////////////////////////
ArrayUint8::ArrayUint8( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, size_t numElements, bool decimalFormat)
:Array<uint8_t>( myModelBase, staticInfo, numElements )
, m_decimal( decimalFormat )
{
}

ArrayUint8::ArrayUint8( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, size_t numElements, const uint8_t* srcData, bool decimalFormat )
    :Array<uint8_t>( myModelBase, staticInfo, numElements, srcData )
    , m_decimal( decimalFormat )
{
}


///////////////////////////////////////////////////////////////////////////////
int8_t ArrayUint8::read( uint8_t* dstData, size_t dstNumElements, size_t srcIndex, uint16_t* seqNumPtr ) const throw()
{
    InternalData dst ={ dstData, dstNumElements, srcIndex };
    return ModelPointCommon_::read( &dst, sizeof( dst ), seqNumPtr );
}

uint16_t ArrayUint8::write( uint8_t* srcData, size_t srcNumElements, LockRequest_T lockRequest, size_t dstIndex ) throw()
{
    InternalData src ={ srcData, srcNumElements, dstIndex };
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
    return m_decimal ? "Cpl::Dm::Mp::ArrayUint8-dec" : "Cpl::Dm::Mp::ArrayUint8-hex";
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
        dst.formatOpt( append, "%lu%c0", (unsigned long) m_data.numElements, OPTION_CPL_RTE_MODEL_POINT_ELEM_DELIMITER_CHAR );

        // DECIMAL format
        if ( m_decimal )
        {
            // Output the elements
            for ( size_t idx=0; idx < m_data.numElements; idx++ )
            {
                dst.formatOpt( true, "%c%d", OPTION_CPL_RTE_MODEL_POINT_ELEM_DELIMITER_CHAR, m_data.elemPtr[idx] );
            }
        }

        // HEX format
        else
        {
            dst += OPTION_CPL_RTE_MODEL_POINT_ELEM_DELIMITER_CHAR;
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
    // Parse the number of elements and starting index 
    unsigned long numElements;
    unsigned long startIndex;
    const char*   fullSrcText = srcText;
    const char*   srcTextPtr  = parseNumElementsAndStartingIndex( srcText, errorMsg, retSequenceNumber, numElements, startIndex );
    if ( srcTextPtr == 0 )
    {
        return 0;
    }

    // Get exclusive access to the global parse buffer
    ModelDatabase::globalLock_();
    uint8_t*      tempArray = (uint8_t*) ModelDatabase::g_parseBuffer_;
    unsigned long elemIndex = startIndex;
    unsigned long elemNum   = 0;

    // DECIMAL parse
    if ( m_decimal )
    {
        while ( elemIndex < m_data.numElements && *srcTextPtr != '\0' && (terminationChars == 0 || strchr( terminationChars, *srcTextPtr ) == 0) )
        {
            // Parse the next element
            char        tempTermChars[2] ={ OPTION_CPL_RTE_MODEL_POINT_ELEM_DELIMITER_CHAR, '\0' };
            unsigned    elemValue;
            const char* endptr;
            if ( Cpl::Text::a2ui( elemValue, srcTextPtr, 10, tempTermChars, &endptr ) == false )
            {
                return processError( errorMsg, retSequenceNumber, "Failed to parse element number %lu. [%s]", elemNum, fullSrcText );
            }

            // Update the Model Point's data
            else
            {
                tempArray[elemNum] = (uint8_t) elemValue;
                srcTextPtr         = endptr + 1;
                elemIndex++;
                elemNum++;
            }

        }

        // If I get here, the I was successful in parsing the string , however
        // the srcTextPtr is pointing past the null terminate (because the loop
        // logic is expecting a trailing ':' on the last element) - so 
        // correct the srcTextPtr pointer
        srcTextPtr--;
    }

    // HEX Parse
    else
    {
        while ( elemIndex < m_data.numElements && *srcTextPtr != '\0' && (terminationChars == 0 || strchr( terminationChars, *srcTextPtr ) == 0) )
        {
            // Parse the next element
            char        tempText[3] ={ *srcTextPtr, *(srcTextPtr + 1), '\0' };
            unsigned    elemValue;
            const char* endptr;
            if ( Cpl::Text::a2ui( elemValue, tempText, 16, 0, &endptr ) == false )
            {
                return processError( errorMsg, retSequenceNumber, "Failed to parse element number %lu. [%s]", elemNum, fullSrcText );
            }

            // Update the Model Point's data
            else
            {
                tempArray[elemNum] = (uint8_t) elemValue;
                srcTextPtr        += 2;
                elemIndex++;
                elemNum++;
            }
        }
    }

    // Check for correct number of elements
    if ( elemNum != numElements )
    {
        return processError( errorMsg, retSequenceNumber, "Missing elements. Num elements found=%lu, expected=%lu [%s]", elemNum, numElements, fullSrcText );
    }

    // Update the Model Point and release my lock on the global parse buffer
    uint16_t seqnum = write( tempArray, numElements, lockAction, startIndex );
    ModelDatabase::globalLock_();

    // Set the return sequence number
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    // Return the remaining string
    return srcTextPtr;
}


