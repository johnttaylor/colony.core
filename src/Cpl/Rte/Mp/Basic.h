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
    Array( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, int8_t validState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID, ELEMTYPE* srcData=0 )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, validState )
        , m_data( { new(std::nothrow) ELEMTYPE[numElements], numElements, 0 } )
    {
        // Check for the case of failed memory allocation
        if ( m_data.elemPtr == 0 )
        {
            m_data.numElements = 0;
        }

        // Initialize the Array when being initialized to the VALID state
        if ( ModelPoint::IS_VALID( validState ) && m_data.numElements != 0 )
        {
            // Zero the array if no data provide
            if ( srcData == 0  )
            {
                memset( data.elemPtr, 0, m_data.NumElements );
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
        CPL_SYSTEM_ASSERT( dstSize == sizeof( Data ) );
        InternalData* dstInfo = (InternalData*) dstDst;

        // Make sure we don't read past the m_data storage
        if ( m_data.numElements == 0 )
        {
            dstInfo->numElements = 0;
        }
        else if ( dstInfo->elemIndex + dstInfo->numElements > m_data.numElements )
        {
            dstInfo->numElements = m_data.numElements - dstInfo->elemIndex
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
        InternalData* otherInfo = (InternalData*) srcData;

        // Make sure we don't compare past the m_data storage
        if ( m_data.numElements == 0 )
        {
            otherInfo->numElements = 0;
        }
        else if ( srcInfo->elemIndex + srcInfo->numElements > m_data.numElements )
        {
            otherInfo->numElements = m_data.numElements - otherInfo->elemIndex;
        }

        return memcmp( &(m_data.elemPtr[srcInfo->elemIndex]), otherInfo->elemPtr, otherInfo->numElements * sizeof( ELEMTYPE ) ) == 0;
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
};


};      // end namespaces
};
};
#endif  // end header latch
