#ifndef Cpl_Rte_Mp_ArrayUint8_h_
#define Cpl_Rte_Mp_ArrayUint8_h_
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


#include "Cpl/Rte/M/Basic.h"
#include "Cpl/System/Assert.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Rte {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    Array of uint8_t.

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
template<int SIZE>
class ArrayUint8 : public Array<uint8_t,SIZE>
{
protected:
    /// The element's value
    ELEMTYPE    m_data[SIZE];


public:
    /** Constructor.  If the 'srcData' pointer is set to zero, then the entire
        array will be initialized to zero.  If the 'srcData' is not zero, the
        caller must provide the length of 'srcData' in terms of number of
        elements (not in bytes).
     */
    Array( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, int8_t validState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID, ELEMTYPE* srcData=0, size_t srcNumElements=0 )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, validState )
        , m_data( initialValue ) {}


public:
    /// See Cpl::Rte::ModelPoint.  This method IS thread safe.
    size_t getSize() const throw()
    {
        return sizeof( m_data );
    }

public:
    /// See Cpl::Rte::ModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const throw()
    {
        CPL_SYSTEM_ASSERT( dstSize >= sizeof( m_data ) );
        memcpy( dstData, m_data, sizeof( m_data ) );
    }

    /// See Cpl::Rte::ModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) throw()
    {
        CPL_SYSTEM_ASSERT( srcSize <= sizeof( m_data ) );
        memcpy( m_data, srcData, srcSize );
        m_data = *((ELEMTYPE*) srcData);
    }

    /// See Cpl::Rte::ModelPoint.  
    bool isDataEqual_( const void* otherData ) const throw()
    {
        return memcmp( m_data, otherData, sizeof( m_data ) ) == 0;
    }

    /// See Cpl::Rte::Point.  
    const void* getImportExportDataPointer_() const throw()
    {
        return (const void*) m_data;
    }

    size_t getImportExportSize_() const throw()
    {
        return sizeof( m_data );
    }
};




};      // end namespaces
};
};
#endif  // end header latch
