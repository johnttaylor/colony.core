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
        :ModelPointCommon_(myModelBase, &m_data, staticInfo, validState )
        , m_data( initialValue ) {}


public:
    /// See Cpl::Rte::ModelPoint.  This method IS thread safe.
    size_t getSize() const throw()
    {
        return sizeof( ELEMTYPE );
    }

public:
    /// See Cpl::Rte::ModelPoint
    void copyDataFrom_( const void* srcData ) throw()
    {
        m_data = *((ELEMTYPE*) srcData);
    }

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

    /// See Cpl::Rte::Point.  The default implementation is for integers
    bool isDataEqual_( const void* otherData ) const throw()
    {
        ELEMTYPE left = *((ELEMTYPE*) otherData);
        return m_data == left;
    }

    /// See Cpl::Rte::Point.  
    void* getDataPointer_() const throw()
    {
        return (void*)(&m_data);
    }

};




};      // end namespaces
};
};
#endif  // end header latch
