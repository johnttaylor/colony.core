#ifndef Cpl_Rte_Mp_Basic_h_
#define Cpl_Rte_Mp_Basic_h_
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


#include "Cpl/Dm/ModelPointCommon_.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include <string.h>


///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C primitive type of type: 'ELEMTYPE'.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
        documented otherwise.
 */
template<class ELEMTYPE>
class Basic : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// The element's value
    ELEMTYPE    m_data;

public:
    /// Constructor: Invalid MP
    Basic( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    Basic( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, ELEMTYPE initialValue )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, Cpl::Dm::ModelPoint::MODEL_POINT_STATE_VALID )
    {
        m_data = initialValue;
    }

public:
	/// Type safe read. See Cpl::Dm::ModelPoint
	virtual int8_t read(ELEMTYPE& dstData, uint16_t* seqNumPtr = 0) const noexcept
	{
		return ModelPointCommon_::read(&dstData, sizeof(ELEMTYPE), seqNumPtr);
	}

	/// Type safe write. See Cpl::Dm::ModelPoint
	virtual uint16_t write(ELEMTYPE newValue, LockRequest_T lockRequest = eNO_REQUEST) noexcept
	{
		return ModelPointCommon_::write(&newValue, sizeof(ELEMTYPE), lockRequest);
	}


public:
    /// See Cpl::Dm::ModelPoint.  This method IS thread safe.
    size_t getSize() const noexcept
    {
        return sizeof( ELEMTYPE );
    }

public:
    /// See Cpl::Dm::ModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
    {
        CPL_SYSTEM_ASSERT( dstSize == sizeof( ELEMTYPE ) );
        *((ELEMTYPE*) dstData) = m_data;
    }

    /// See Cpl::Dm::ModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
    {
        CPL_SYSTEM_ASSERT( srcSize == sizeof( ELEMTYPE ) );
        m_data = *((ELEMTYPE*) srcData);
    }

    /// See Cpl::Dm::ModelPoint.  The default implementation is for integers
    bool isDataEqual_( const void* otherData ) const noexcept
    {
        ELEMTYPE left = *((ELEMTYPE*) otherData);
        return m_data == left;
    }

    /// See Cpl::Dm::Point.  
    const void* getImportExportDataPointer_() const noexcept
    {
        return (const void*) (&m_data);
    }

    /// See Cpl::Dm::Point.  
    size_t getInternalDataSize() const noexcept
    {
        return sizeof( ELEMTYPE );
    }
};

/** This template class extends the implementation of Basic<ELEMTYPE> to support
    the toJSON() and fromJSON_() methods for integer types.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
           documented otherwise.
 */
template<class ELEMTYPE>
class BasicInteger : public Basic<ELEMTYPE>
{
protected:
	/// Flag for to/from json() methods
	bool        m_decimal;

public:
    /// Constructor: Invalid MP
    BasicInteger( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, bool decimalFormat=true )
        :Basic<ELEMTYPE>( myModelBase, staticInfo )
		, m_decimal(decimalFormat)
    {
    }

    /// Constructor: Valid MP (requires initial value)
    BasicInteger( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, ELEMTYPE initialValue, bool decimalFormat=true )
        :Basic<ELEMTYPE>( myModelBase, staticInfo, initialValue )
		, m_decimal(decimalFormat)
	{
    }

public:
    /// See Cpl::Dm::Point.  
    bool toJSON( char* dst, size_t dstSize, bool& truncated ) noexcept
    {
        // Get a snapshot of the my data and state
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        ELEMTYPE value  = Basic<ELEMTYPE>::m_data;
        uint16_t seqnum = Basic<ELEMTYPE>::m_seqNum;
        int8_t   valid  = Basic<ELEMTYPE>::m_validState;
        bool     locked = Basic<ELEMTYPE>::m_locked;
		Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();

        // Start the conversion
        JsonDocument& doc = Cpl::Dm::ModelPointCommon_::beginJSON( valid, locked, seqnum );

        // Construct the 'val' key/value pair (as a simple numeric)
        if (Cpl::Dm::ModelPointCommon_::IS_VALID( valid ) )
        {
            if ( m_decimal )
            {
                doc["val"] = value;
            }

            // Construct the 'val' key/value pair (as a HEX string)
            else
            {
                Cpl::Text::FString<20> tmp;
                tmp.format( "0x%llX", (unsigned long long) value );
                doc["val"] = (char*) tmp.getString();
            }
        }

        // End the conversion
		Cpl::Dm::ModelPointCommon_::endJSON( dst, dstSize, truncated );
        return true;
    }

    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
    {
        ELEMTYPE newValue = 0;

        // Attempt to parse the value key/value pair (as a simple numeric)
        if ( m_decimal )
        {
            ELEMTYPE checkForError = src | (ELEMTYPE) 2;
            newValue               = src | (ELEMTYPE) 1;
            if ( newValue == (ELEMTYPE) 1 && checkForError == (ELEMTYPE) 2 )
            {
                if ( errorMsg )
                {
                    *errorMsg = "Invalid syntax for the 'val' key/value pair";
                }
                return false;
            }
        }

        // Attempt to parse the value as HEX string
        else
        {
            const char*        val = src;
            unsigned long long value;
            if ( Cpl::Text::a2ull( value, val, 16 ) == false )
            {
                if ( errorMsg )
                {
                    *errorMsg = "Invalid syntax for the 'val' key/value pair";
                }
                return false;
            }

            newValue = (ELEMTYPE) value;	 
        }

        retSequenceNumber = Basic<ELEMTYPE>::write( newValue, lockRequest );
        return true;
    }
};

#if 0
/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a array of a C primitive type of type: 'ELEMTYPE'.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
           documented otherwise.

    Template args:
        ELEMTYPE    - Type of the array element
 */
template<class ELEMTYPE>
class Array : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// The MP's Internal Data container 
    typedef struct
    {
        ELEMTYPE* elemPtr;        //!< Pointer to the element in the array to read/write
        size_t    numElements;    //!< Number of element to read/write
        size_t    elemIndex;      //!< Starting array index
    } InternalData;

protected:
    /// The element's value
    InternalData     m_data;

    /// Flag for to/from json() methods
    bool m_decimal;

public:
    /// Constructor: Invalid MP
    Array( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, bool decimalFormat=true )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID )
        , m_data( { new(std::nothrow) ELEMTYPE[numElements], numElements, 0 } )
        , m_decimal( decimalFormat )
    {
        // Throw a fatal error if global parse buffer is too small
        if ( OPTION_CPL_DM_MODEL_DATABASE_TEMP_STORAGE_SIZE < numElements * sizeof( ELEMTYPE ) )
        {
            Cpl::System::FatalError::logf( "Cpl::Dm::Array().  Creating a Array of size %lu which is greater than the fromJSON_() temporary buffer", numElements * sizeof( ELEMTYPE ) );
        }

        // Check for the case of failed memory allocation
        if ( m_data.elemPtr == 0 )
        {
            m_data.numElements = 0;
        }

        // Initialize the array to all zero - so as to have deterministic 'invalid' value 
        memset( m_data.elemPtr, 0, m_data.numElements * sizeof( ELEMTYPE ) );
    }


    /** Constructor.  Valid MP.  Requires an initial value.  If the 'srcData'
        pointer is set to zero, then the entire array will be initialized to
        zero.   Note: 'srcData' MUST contain at least 'numElements' elements.
     */
    Array( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, const ELEMTYPE* srcData, bool decimalFormat=true )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, Cpl::Dm::ModelPoint::MODEL_POINT_STATE_VALID )
        , m_data( { new(std::nothrow) ELEMTYPE[numElements], numElements, 0 } )
        , m_decimal( decimalFormat )
    {
        // Throw a fatal error if global parse buffer is too small
        if ( OPTION_CPL_DM_MODEL_DATABASE_TEMP_STORAGE_SIZE < numElements * sizeof( ELEMTYPE ) )
        {
            Cpl::System::FatalError::logf( "Cpl::Dm::Array().  Creating a Array of size %lu which is greater than the fromJSON_() temporary buffer", numElements * sizeof( ELEMTYPE ) );
        }

        // Check for the case of failed memory allocation
        if ( m_data.elemPtr == 0 )
        {
            m_data.numElements = 0;
        }

        // Initialize the Array 
        if ( m_data.numElements != 0 )
        {
            // Zero the array if no data provide 
            if ( srcData == 0 )
            {
                memset( m_data.elemPtr, 0, m_data.numElements * sizeof( ELEMTYPE ) );
            }

            // Initialize the array to the provided values
            else
            {
                memcpy( m_data.elemPtr, srcData, m_data.numElements * sizeof( ELEMTYPE ) );
            }
        }
    }

    /// Destructor (free up allocate array memory)
    ~Array()
    {
        delete m_data.elemPtr;
    }

public:
	/** Type safe read. See Cpl::Dm::ModelPoint.

	The caller can read a subset of array starting from the specified index
	in the Model Point's array.  Note: if srcIndex + dstNumElements exceeds
	the size of the MP's data then the read operation will be truncated.
	*/
	virtual int8_t read(ELEMTYPE* dstData, size_t dstNumElements, size_t srcIndex = 0, uint16_t* seqNumPtr = 0) const noexcept
	{
		InternalData dst = { dstData, dstNumElements, srcIndex };
		return ModelPointCommon_::read(&dst, sizeof(dst), seqNumPtr);
	}

	/** Type safe write. See Cpl::Dm::ModelPoint.

	The caller can write a subset of array starting from the specified index
	in the Model Point's array.  Note: if dstIndex + srcNumElements exceeds
	the size of the MP's data then the write operation will be truncated

	NOTE: The application/caller is responsible for what a 'partial write'
	means to the integrity of the MP's data.  WARNING: Think before
	doing a partial write!  For example, if the MP is in the invalid
	state and a partial write is done - then the MP's data/array is
	only partially initialized AND then MP is now in the valid
	state!
	*/

	virtual uint16_t write(ELEMTYPE* srcData, size_t srcNumElements, LockRequest_T lockRequest = eNO_REQUEST, size_t dstIndex = 0) noexcept
	{
		InternalData src = { srcData, srcNumElements, dstIndex };
		return ModelPointCommon_::write(&src, sizeof(src), lockRequest);
	}

public:
    /// See Cpl::Dm::ModelPoint.  This method IS thread safe.
    size_t getSize() const noexcept
    {
        return m_data.numElements * sizeof( ELEMTYPE );
    }

public:
    /// See Cpl::Dm::ModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
    {
        CPL_SYSTEM_ASSERT( dstSize == sizeof( m_data ) );
        InternalData* dstInfo = (InternalData*) dstData;

        // Make sure we don't read past the m_data storage
        if ( m_data.numElements == 0 )
        {
            dstInfo->numElements = 0;
        }
        else if ( dstInfo->elemIndex + dstInfo->numElements > m_data.numElements )
        {
            dstInfo->numElements = m_data.numElements - dstInfo->elemIndex;
        }

        // Copy the data to 'dst'
        memcpy( dstInfo->elemPtr, &(m_data.elemPtr[dstInfo->elemIndex]), dstInfo->numElements * sizeof( ELEMTYPE ) );
    }

    /// See Cpl::Dm::ModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
    {
        CPL_SYSTEM_ASSERT( srcSize == sizeof( m_data ) );
        InternalData* srcInfo = (InternalData*) srcData;

        // Make sure we don't write past the m_data storage
        if ( m_data.numElements == 0 )
        {
            srcInfo->numElements = 0;
        }
        else if ( srcInfo->elemIndex + srcInfo->numElements > m_data.numElements )
        {
            srcInfo->numElements = m_data.numElements - srcInfo->elemIndex;
        }

        // Copy the data to 'src'
        memcpy( &(m_data.elemPtr[srcInfo->elemIndex]), srcInfo->elemPtr, srcInfo->numElements * sizeof( ELEMTYPE ) );
    }

    /// See Cpl::Dm::ModelPoint.  
    bool isDataEqual_( const void* otherData ) const noexcept
    {
        InternalData* otherInfo = (InternalData*) otherData;

        // Make sure we don't compare past the m_data storage
        if ( m_data.numElements == 0 )
        {
            otherInfo->numElements = 0;
        }
        else if ( otherInfo->elemIndex + otherInfo->numElements > m_data.numElements )
        {
            otherInfo->numElements = m_data.numElements - otherInfo->elemIndex;
        }

        return memcmp( &(m_data.elemPtr[otherInfo->elemIndex]), otherInfo->elemPtr, otherInfo->numElements * sizeof( ELEMTYPE ) ) == 0;
    }

    /// See Cpl::Dm::Point.  
    const void* getImportExportDataPointer_() const noexcept
    {
        return (const void*) m_data.elemPtr;
    }

    /// See Cpl::Dm::Point.  
    size_t getInternalDataSize() const noexcept
    {
        return m_data.numElements * sizeof( ELEMTYPE );
    }

public:
    /// See Cpl::Dm::Point.  
    bool toJSON( char* dst, size_t dstSize, bool& truncated ) noexcept
    {
        // Lock the Model Point
        m_modelDatabase.lock_();
        uint16_t seqnum = m_seqNum;
        int8_t   valid  = m_validState;
        bool     locked = m_locked;

        // Start the conversion
        JsonDocument& doc = beginJSON( valid, locked, seqnum );

        // Construct the 'val' array object
        if ( IS_VALID( valid ) )
        {
            JsonObject obj = doc.createNestedObject( "val" );
            obj["start"] = 0;
            JsonArray arr = obj.createNestedArray( "elems" );
            for ( size_t i=0; i < m_data.numElements; i++ )
            {
                if ( m_decimal )
                {
                    arr.add( m_data.elemPtr[i] );
                }
                else
                {
                    Cpl::Text::FString<20> s;
                    s.format( "0x%llX", (unsigned long long) m_data.elemPtr[i] );
                    arr.add( (char*) s.getString() );
                }
            }
        }

        // End the conversion
        endJSON( dst, dstSize, truncated );

        // unlock the database
        m_modelDatabase.unlock_();
        return true;
    }

    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
    {
        // Check for object
        if ( src.is<JsonObject>() == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "'val' key/value pair is NOT an JSON object";
            }
            return false;
        }

        // Check for embedded array
        JsonArray elems = src["elems"];
        if ( elems.isNull() )
        {
            if ( errorMsg )
            {
                *errorMsg = "'val' key/value pair is missing the embedded 'elems' array";
            }
            return false;
        }

        // Get starting index (note: if not present a default of '0' will be returned)
        size_t startIdx = src["start"];

        // Check for exceeding array limits
        size_t numElements = elems.size();
        if ( numElements > m_data.numElements )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Number of array elements (%lu) exceeds the MP's element count (%lu)", src.size(), m_data.numElements );
            }
            return false;
        }

        // Attempt to parse the value key/value pair (as a simple numeric)
        ELEMTYPE* tempArray = (ELEMTYPE*) ModelDatabase::g_tempBuffer_;
        for ( size_t i = 0; i < numElements; i++ )
        {
            // Attempt to parse the value as simple numeric
            if ( m_decimal )
            {
                tempArray[i] = elems[i];
            }

            // Attempt to parse the value as HEX string
            else
            {
                const char*        val = elems[i];
                unsigned long long value;
                if ( Cpl::Text::a2ull( value, val, 16 ) == false )
                {
                    if ( errorMsg )
                    {
                        *errorMsg = "Invalid syntax for the 'val' key/value pair";
                    }
                    return false;
                }
                tempArray[i] = (ELEMTYPE) value;
            }      
        }

        // Update the Model Point 
        InternalData srcArray = { tempArray, numElements, startIdx };
        retSequenceNumber = ModelPointCommon_::write( &srcArray, sizeof( srcArray ), lockRequest );
        return true;
    }
};
#endif

};      // end namespaces
};
};
#endif  // end header latch
