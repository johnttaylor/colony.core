#ifndef Cpl_Rte_Mp_Basic_h_
#define Cpl_Rte_Mp_Basic_h_
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


#include "Cpl/Rte/ModelPointCommon_.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/atob.h"
#include <string.h>

///
namespace Cpl {
///
namespace Rte {
///
namespace Mp {


/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C primitive type of type: 'ELEMTYPE'.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
        documented otherwise.
 */
template<class ELEMTYPE>
class Basic : public Cpl::Rte::ModelPointCommon_
{
protected:
    /// The element's value
    ELEMTYPE    m_data;


public:
    /// Constructor
    Basic( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, ELEMTYPE initialValue = (ELEMTYPE) 0, int8_t validState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, validState )
        , m_data( initialValue ) {}


public:
    /// See Cpl::Rte::ModelPoint.  This method IS thread safe.
    size_t getSize() const throw()
    {
        return sizeof( ELEMTYPE );
    }

public:
    /// See Cpl::Rte::ModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const throw()
    {
        CPL_SYSTEM_ASSERT( dstSize == sizeof( ELEMTYPE ) );
        *((ELEMTYPE*) dstData) = m_data;
    }

    /// See Cpl::Rte::ModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) throw()
    {
        CPL_SYSTEM_ASSERT( srcSize == sizeof( ELEMTYPE ) );
        m_data = *((ELEMTYPE*) srcData);
    }

    /// See Cpl::Rte::ModelPoint.  The default implementation is for integers
    bool isDataEqual_( const void* otherData ) const throw()
    {
        ELEMTYPE left = *((ELEMTYPE*) otherData);
        return m_data == left;
    }

    /// See Cpl::Rte::Point.  
    const void* getImportExportDataPointer_() const throw()
    {
        return (const void*) (&m_data);
    }

    size_t getImportExportSize_() const throw()
    {
        return sizeof( ELEMTYPE );
    }
};


/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a array of a C primitive type of type: 'ELEMTYPE'.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
           documented otherwise.

    Template args:
        ELEMTYPE    - Type of the array element
 */
template<class ELEMTYPE>
class Array : public Cpl::Rte::ModelPointCommon_
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

public:
    /** Constructor.  If the 'srcData' pointer is set to zero, then the entire
        array will be initialized to zero.   Note: 'srcData' MUST contain
        at least 'numElements' elements.
     */
    Array( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, int8_t validState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID, const ELEMTYPE* srcData=0 )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, validState )
        , m_data( { new(std::nothrow) ELEMTYPE[numElements], numElements, 0 } )
    {
        // Throw a fatal error if global parse buffer is too small
        if ( OPTION_CPL_RTE_MODEL_DATABASE_MAX_LENGTH_FROM_STRING_BUFFER < numElements * sizeof( ELEMTYPE ) )
        {
            Cpl::System::FatalError::logf( "Cpl::Rte::Array().  Creating a Array of size %lu which is greater than the fromString() parser buffer", numElements * sizeof( ELEMTYPE ) );
        }

        // Check for the case of failed memory allocation
        if ( m_data.elemPtr == 0 )
        {
            m_data.numElements = 0;
        }

        // Initialize the Array 
        if ( m_data.numElements != 0 )
        {
            // Zero the array if no data provide OR if the initial state is invalid.  
            // Setting the invalid-initial state to zero is to at least always provide 
            // consist initial values for the MP if the first write that the application 
            // does is a PARTIAL write (another reason to NOT do partial writes)
            if ( srcData == 0 ||  ModelPoint::IS_VALID( validState ) == false )
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
    /// See Cpl::Rte::ModelPoint.  This method IS thread safe.
    size_t getSize() const throw()
    {
        return m_data.numElements * sizeof( ELEMTYPE );
    }

public:
    /// See Cpl::Rte::ModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const throw()
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

    /// See Cpl::Rte::ModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) throw()
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

    /// See Cpl::Rte::ModelPoint.  
    bool isDataEqual_( const void* otherData ) const throw()
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

    /// See Cpl::Rte::Point.  
    const void* getImportExportDataPointer_() const throw()
    {
        return (const void*) m_data.elemPtr;
    }

    size_t getImportExportSize_() const throw()
    {
        return m_data.numElements * sizeof( ELEMTYPE );
    }


protected:
    /// Helper method when processing errors that occur during setFromText() method
    const char* processError( Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber, const char* errorFormatText, ... )
    {
        // Update the Caller's error Message (if provided)
        va_list ap;
        va_start( ap, errorFormatText );
        if ( errorMsg )
        {
            errorMsg->vformat( errorFormatText, ap );
        }

        va_end( ap );

        // Lock the database 
        m_modelDatabase.lock_();

        // Update the caller's sequence number (if provided)
        if ( retSequenceNumber )
        {
            *retSequenceNumber = m_seqNum;
        }

        m_modelDatabase.unlock_();

        // Return 'parse failed'
        return 0;
    }

    /// Helper method to for setFromText() implementation
    const char* parseNumElementsAndStartingIndex( const char* srcText, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber, unsigned long& numElements, unsigned long& startIndex ) throw()
    {
        // Parse numElements and starting index
        const char*   endptr;
        char          tempTermChars[2] = { OPTION_CPL_RTE_MODEL_POINT_ELEM_DELIMITER_CHAR, '\0' };
        if ( Cpl::Text::a2ul( numElements, srcText, 10, tempTermChars, &endptr ) == false )
        {
            return processError( errorMsg, retSequenceNumber, "Unable to parse the numElems field.  Format is: <numElems>:<mpIndex>:<e0> [%s]", srcText );
        }
        const char*   endptr2;
        if ( Cpl::Text::a2ul( startIndex, endptr+1, 10, tempTermChars, &endptr2 ) == false )
        {
            return processError( errorMsg, retSequenceNumber, "Unable to parse the mpIndex field.  Format is: <numElems>:<mpIndex>:<e0> [%s]", srcText );
        }
        if ( numElements + startIndex > m_data.numElements )
        {
            return processError( errorMsg, retSequenceNumber, "Unable to parse the numElems+mpIndex exceeds the size of the array (array size=%lu) [%s]", (unsigned long) m_data.numElements, srcText );
        }

        return endptr2+1;
    }

};


};      // end namespaces
};
};
#endif  // end header latch
