#ifndef Cpl_Rte_Point_Basic_h_
#define Cpl_Rte_Point_Basic_h_
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


#include "Cpl/Rte/Point.h"
#include <string.h>

///
namespace Cpl {
///
namespace Rte {
///
namespace Point {


/** This template class provides a mostly concrete implementation for a Point
    who's data is a C primitive type of type: 'ELEMTYPE'.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
           documented otherwise.
 */
template<class ELEMTYPE>
class PointBasic : public Cpl::Rte::Point
{
protected:
    /// The element's value
    ELEMTYPE    m_data;


public:
    /// Constructor
    PointBasic( ELEMTYPE initialValue = (ELEMTYPE) 0 )
        :m_data( initialValue ) {}


public:
    /// See Cpl::Rte::Point
    size_t getSize() const throw()
    {
        return sizeof( ELEMTYPE );
    }

    /// See Cpl::Rte::Point
    size_t export(void* dstDataStream) const throw()
    {   
        size_t result = 0;
        if ( dstDataStream )
        {
            result = sizeof( data );
            memcpy( dstDataStream, &m_data, result );
        }
        return result;
    }

    /// See Cpl::Rte::Point
    size_t import( const void* srcDataStream ) throw()
    {   
        size_t result = 0;
        if ( srcDataStream )
        {
            result = sizeof( data );
            memcpy( &m_data, srcDataStream, result );
        }
        return result;
    }

    /// See Cpl::Rte::Point
    virtual size_t getExternalSize( void ) const
    {
        return sizeof( ELEMTYPE );
    }

public:
    /// Data accessor
    ELEMTYPE getValue()
    {
        return m_data;
    }

    /// Data accessor
    void setValue( ELEMTYPE newValue )
    {
        m_data = newValue;
    }


protected:
    /// See Cpl::Rte::Point
    void copyFrom_( const Point& src ) throw()
    {
        m_data = src.m_data;
    }

    /// See Cpl::Rte::Point.  The default implementation is for integers
    bool isEqual_( const Point& other ) const throw()
    {
        ELEMTYPE left = *((ELEMTYPE*) other.getDataPointer());
        return m_data == left;
    }

    /// See Cpl::Rte::Point.  
    void* getDataPointer() const throw()
    {
        return &m_data;
    }

};




};      // end namespaces
};
};
#endif  // end header latch
