#ifndef Cpl_Dm_Mp_Array_h_
#define Cpl_Dm_Mp_Array_h_
/*-----------------------------------------------------------------------------
 * This file is part of the Colony.Core Project.  The Colony.Core Project is an
 * open source project with a BSD type of licensing agreement.  See the license
 * agreement (license.txt) in the top/ directory or on the Internet at
 * http://integerfox.com/colony.core/license.txt
 *
 * Copyright (c) 2014-2025  John T. Taylor
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Dm/ModelPointCommon_.h"


/** The number of Elements in the temporary array (that is allocated on the
    STACK) when parsing the array elements in the fromJSON_() method.
 */
#ifndef OPTION_CPL_DM_MP_ARRAY_TEMP_ARRAY_NUM_ELEMENTS
#define OPTION_CPL_DM_MP_ARRAY_TEMP_ARRAY_NUM_ELEMENTS 8
#endif


///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This a mostly concrete class provides 'common' implementation for a Model
    Point who's data is a array of elements

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false seqnum:nnnn, locked:true|false, val:{start:<firstIndex>,elems:[<elemN>,<elemN+1>,...]}}" }

    \endcode

 */
class ArrayBase_ : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// Meta data for read/write/copy operations
    struct MetaData_T
    {
        uint8_t* elemPtr;      //!< Pointer to the 1st element in the array to read/write
        size_t   numElements;  //!< Number of element to read/write
        size_t   elemIndex;    //!< Starting array index
    };

protected:
    /// Number of elements in the array
    size_t m_numElements;

    /// Size, in bytes, of an element
    size_t m_elementSize;

protected:
    /// Constructor: Invalid MP
    ArrayBase_( Cpl::Dm::ModelDatabase& myModelBase,
                const char*             symbolicName,
                void*                   myDataPtr,
                size_t                  numElements,
                size_t                  elementSize );


    /** Constructor.  Valid MP.  Requires an initial value.  If the 'initialValueSrcPtr'
        pointer is set to zero, then the entire array will be initialized to
        zero.   Note: The array that 'initialValueSrcPtr' points to ' MUST contain
        at least 'numElements' elements.
     */
    ArrayBase_( Cpl::Dm::ModelDatabase& myModelBase,
                const char*             symbolicName,
                void*                   myDataPtr,
                size_t                  numElements,
                size_t                  elementSize,
                void*                   initialValueSrcPtr );

protected:
    /** The caller can read a subset of array starting from the specified index
        in the Model Point's array.  Note: if srcIndex + dstNumElements exceeds
        the size of the MP's data then the read operation will be truncated.
    */
    virtual bool readArrayElements( void* dstData, size_t dstNumElements, size_t srcIndex = 0, uint16_t* seqNumPtr = 0 ) const noexcept;

    /** The caller can write a subset of array starting from the specified index
        in the Model Point's array.  Note: if dstIndex + srcNumElements exceeds
        the size of the MP's data then the write operation will be truncated

        NOTE: The application/caller is responsible for what a 'partial write'
        means to the integrity of the MP's data.  WARNING: Think before
        doing a partial write!  For example, if the MP is in the invalid
        state and a partial write is done - then the MP's data/array is
        only partially initialized AND then MP is now in the valid
        state!
    */
    virtual uint16_t writeArrayElements( const void* srcData, size_t srcNumElements, size_t dstIndex = 0, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept;

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    virtual uint16_t copyArrayFrom( const ArrayBase_& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept;

public:
    /// Returns the number of element in the array. This method IS thread safe.
    inline size_t getNumElements() const noexcept
    {
        return m_numElements;
    }

public:
    /// See Cpl::Dm::ModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept;

    /// See Cpl::Dm::ModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept;

    /// See Cpl::Dm::ModelPoint.
    bool isDataEqual_( const void* otherData ) const noexcept;


    /// See Cpl::Dm::Point.
    size_t getInternalDataSize_() const noexcept;

    /// See Cpl::Dm::ModelPoint.
    bool importMetadata_( const void* srcDataStream, size_t& bytesConsumed ) noexcept;

    /// See Cpl::Dm::ModelPoint.
    bool exportMetadata_( void* dstDataStream, size_t& bytesAdded ) const noexcept;
};

/** This template class extends the implementation of ArrayBase_ to support
    the toJSON() and fromJSON_() methods for numeric element types.

    NOTES:
    1) All methods in this class are NOT thread Safe unless explicitly
    documented otherwise.
*/
template <class ELEMTYPE>
class NumericArrayBase_ : public ArrayBase_
{
protected:
    /// Constructor: Invalid MP
    NumericArrayBase_( Cpl::Dm::ModelDatabase& myModelBase,
                       const char*             symbolicName,
                       ELEMTYPE*               myDataPtr,
                       size_t                  numElements )
        : ArrayBase_( myModelBase, symbolicName, myDataPtr, numElements, sizeof( ELEMTYPE ) )
    {
    }


    /** Constructor.  Valid MP.  Requires an initial value.  If the 'srcData'
        pointer is set to zero, then the entire array will be initialized to
        zero.   Note: 'srcData' MUST contain at least 'numElements' elements.
    */
    NumericArrayBase_( Cpl::Dm::ModelDatabase& myModelBase,
                       const char*             symbolicName,
                       ELEMTYPE*               myDataPtr,
                       size_t                  numElements,
                       ELEMTYPE*               srcData )
        : ArrayBase_( myModelBase, symbolicName, myDataPtr, numElements, sizeof( ELEMTYPE ), srcData )
    {
    }


public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( ELEMTYPE* dstArrray, size_t dstNumElements, size_t srcIndex = 0, uint16_t* seqNumPtr = 0 ) const noexcept
    {
        return ArrayBase_::readArrayElements( dstArrray, dstNumElements, srcIndex, seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( const ELEMTYPE* srcArray, size_t srcNumElements, size_t dstIndex = 0, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        return ArrayBase_::writeArrayElements( srcArray, srcNumElements, dstIndex, lockRequest );
    }

public:
    /** This method is used to read the MP contents and synchronize
        the observer with the current MP contents.  This method should ONLY be
        used in the notification callback method and the 'observerToSync'
        argument MUST be the argument provided by the callback method

        Note: The observer will be subscribed for change notifications after
              this call.
     */
    inline bool readAndSync( ELEMTYPE*      dstArrray,
                             size_t         dstNumElements,
                             SubscriberApi& observerToSync,
                             size_t         srcIndex = 0 )
    {
        uint16_t seqNum;
        return readAndSync( dstArrray, dstNumElements, observerToSync, seqNum , srcIndex);
    }

    /** Same as readAndSync() above, but in addition returns the
        sequence number of the MP.
     */
    inline bool readAndSync( ELEMTYPE*      dstArrray,
                             size_t         dstNumElements,
                             SubscriberApi& observerToSync,
                             uint16_t&      seqNum,
                             size_t         srcIndex = 0 )
    {
        bool result = ArrayBase_::readArrayElements( dstArrray, dstNumElements, srcIndex, &seqNum );
        ArrayBase_::attachSubscriber( observerToSync, seqNum );
        return result;
    }

protected:
    /// See Cpl::Dm::Point.
    void setJSONVal( JsonDocument& doc ) noexcept
    {
        JsonObject obj    = doc.createNestedObject( "val" );
        obj["start"]      = 0;
        JsonArray arr     = obj.createNestedArray( "elems" );
        ELEMTYPE* elemPtr = (ELEMTYPE*)m_dataPtr;
        for ( size_t i = 0; i < m_numElements; i++ )
        {
            arr.add( elemPtr[i] );
        }
    }

public:
    /// See Cpl::Dm::Point.
    bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
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
        size_t startIdx = src["start"] | 0;

        // Check for exceeding array limits
        size_t numElements = elems.size();
        if ( numElements + startIdx > m_numElements )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Number of array elements ([%lu+%lu)] exceeds the MP's element count (%lu)", startIdx, numElements, m_numElements );
            }
            return false;
        }

        // Update the Model Point in 'M' elements at a time (helps to reduce 'noise' on the MP's sequence number)
        size_t offset = 0;
        while ( numElements )
        {
            // Attempt to parse the value key/value pair (as a simple numeric)
            ELEMTYPE tempArray[OPTION_CPL_DM_MP_ARRAY_TEMP_ARRAY_NUM_ELEMENTS];
            size_t   idx;
            for ( idx = 0; idx < numElements && idx < OPTION_CPL_DM_MP_ARRAY_TEMP_ARRAY_NUM_ELEMENTS; idx++ )
            {
                // Is the element syntacticly correct?
                if ( elems[idx].is<ELEMTYPE>() == false )
                {
                    if ( errorMsg )
                    {
                        errorMsg->format( "Failed parsing element[%lu]. Content of the MP is suspect!", offset );
                    }
                    return false;
                }
                tempArray[idx] = elems[idx + offset].as<ELEMTYPE>();
            }
            retSequenceNumber  = ArrayBase_::writeArrayElements( tempArray, idx, startIdx + offset, lockRequest );
            offset            += idx;
            numElements       -= idx;
        }

        return true;
    }
};

/** This mostly concrete template class implements an 'numeric Array' Model Point
    with an element size of N.  A child class is still required. The child classes
    must provide the following:

        getTypeAsText() method and a typedef for child specific 'Observer'
*/
template <class ELEMTYPE, int NUMELEMS, class MPTYPE>
class NumericArray_ : public NumericArrayBase_<ELEMTYPE>
{
protected:
    /// The data store the MP
    ELEMTYPE m_data[NUMELEMS];

protected:
    /// Constructor: Invalid MP
    NumericArray_( Cpl::Dm::ModelDatabase& myModelBase,
                   const char*             symbolicName )
        : NumericArrayBase_<ELEMTYPE>( myModelBase, symbolicName, m_data, NUMELEMS )
    {
    }


    /** Constructor.  Valid MP.  Requires an initial value.  If the 'srcData'
        pointer is set to zero, then the entire array will be initialized to
        zero.   Note: 'srcData' MUST contain at least 'numElements' elements.
    */
    NumericArray_( Cpl::Dm::ModelDatabase& myModelBase,
                   const char*             symbolicName,
                   ELEMTYPE*               srcData )
        : NumericArrayBase_<ELEMTYPE>( myModelBase, symbolicName, m_data, NUMELEMS, srcData )
    {
    }

public:
    /// Updates the MP's data/valid-state from 'src'.
    inline uint16_t copyFrom( const MPTYPE& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        return ArrayBase_::copyArrayFrom( src, lockRequest );
    }

    /// Type safe register observer
    inline void attach( Cpl::Dm::Subscriber<MPTYPE>& observer, uint16_t initialSeqNumber = Cpl::Dm::ModelPoint::SEQUENCE_NUMBER_UNKNOWN ) noexcept
    {
        ArrayBase_::attachSubscriber( observer, initialSeqNumber );
    }

    /// Type safe un-register observer
    inline void detach( Cpl::Dm::Subscriber<MPTYPE>& observer ) noexcept
    {
        ArrayBase_::detachSubscriber( observer );
    }
};

//////////////////////////////////////////////////////////////////////////////
/* The following classes provide concrete numeric Array types for basic types
 */

/// uint8_t Array
template <int N>
class ArrayUint8 : public NumericArray_<uint8_t, N, ArrayUint8<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayUint8( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray_<uint8_t, N, ArrayUint8<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayUint8( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, uint8_t initialValueArray[] )
        : Mp::NumericArray_<uint8_t, N, ArrayUint8<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::ArrayUint8";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ArrayUint8> Observer;
};

/// uint32_t Array
template <int N>
class ArrayUint32 : public NumericArray_<uint32_t, N, ArrayUint32<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayUint32( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray_<uint32_t, N, ArrayUint32<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayUint32( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, uint32_t initialValueArray[] )
        : Mp::NumericArray_<uint32_t, N, ArrayUint32<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::ArrayUint32";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ArrayUint32> Observer;
};

/// uint64_t Array
template <int N>
class ArrayUint64 : public NumericArray_<uint64_t, N, ArrayUint64<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayUint64( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray_<uint64_t, N, ArrayUint64<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayUint64( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, uint64_t initialValueArray[] )
        : Mp::NumericArray_<uint64_t, N, ArrayUint64<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::ArrayUint64";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ArrayUint64> Observer;
};

/// int8_t Array
template <int N>
class ArrayInt8 : public NumericArray_<int8_t, N, ArrayInt8<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayInt8( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray_<int8_t, N, ArrayInt8<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayInt8( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, int8_t initialValueArray[] )
        : Mp::NumericArray_<int8_t, N, ArrayInt8<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::ArrayInt8";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ArrayInt8> Observer;
};

/// int32_t Array
template <int N>
class ArrayInt32 : public NumericArray_<int32_t, N, ArrayInt32<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayInt32( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray_<int32_t, N, ArrayInt32<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayInt32( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, int32_t initialValueArray[] )
        : Mp::NumericArray_<int32_t, N, ArrayInt32<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::ArrayInt32";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ArrayInt32> Observer;
};

/// int64_t Array
template <int N>
class ArrayInt64 : public NumericArray_<int64_t, N, ArrayInt64<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayInt64( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray_<int64_t, N, ArrayInt64<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayInt64( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, int64_t initialValueArray[] )
        : Mp::NumericArray_<int64_t, N, ArrayInt64<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::ArrayInt64";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ArrayInt64> Observer;
};

/// float Array
template <int N>
class ArrayFloat : public NumericArray_<float, N, ArrayFloat<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayFloat( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray_<float, N, ArrayFloat<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayFloat( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, float initialValueArray[] )
        : Mp::NumericArray_<float, N, ArrayFloat<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Cpl::Dm::ModelPofloat.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::ArrayFloat";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ArrayFloat> Observer;
};

/// double Array
template <int N>
class ArrayDouble : public NumericArray_<double, N, ArrayDouble<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayDouble( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray_<double, N, ArrayDouble<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayDouble( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, double initialValueArray[] )
        : Mp::NumericArray_<double, N, ArrayDouble<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Cpl::Dm::ModelPodouble.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::ArrayDouble";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ArrayDouble> Observer;
};

};  // end namespaces
};
};
#endif  // end header latch
