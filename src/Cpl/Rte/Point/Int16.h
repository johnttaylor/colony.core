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
#include "Cpl/Text/atob.h"
#include <string.h>
#include <stdlib.h>

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
    void copyFrom( const Point& src ) throw()
    {
        m_data = src.m_data;
    }

    /// See Cpl::Rte::Point.  The default implementation is for integers
    bool isEqual( const Point& other ) const throw()
    {
        return m_data == other.m_data;
    }
};

///////////////////////////////////////////////////////////////////////////////
class Uint8 : public PointBasic<uint8_t>
{
public:
    /// Constructor
    Uint8( uint8_t initialValue = 0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};

class Int8 : public PointBasic<int8_t>
{
public:
    /// Constructor
    Int8( int8_t initialValue = 0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};


class Uint16 : public PointBasic<uint16_t>
{
public:
    /// Constructor
    Uint16( uint16_t initialValue = 0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};

class Int16 : public PointBasic<int16_t>
{
public:
    /// Constructor
    Int16( int16_t initialValue = 0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};


class Uint32 : public PointBasic<uint32_t>
{
public:
    /// Constructor
    Uint32( uint32_t initialValue = 0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};

class Int32 : public PointBasic<int32_t>
{
public:
    /// Constructor
    Int32( int32_t initialValue = 0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};


class Uint64 : public PointBasic<uint64_t>
{
public:
    /// Constructor
    Uint64( uint64_t initialValue = 0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};

class Int64 : public PointBasic<int64_t>
{
public:
    /// Constructor
    Int64( int64_t initialValue = 0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};


class SizeT : public PointBasic<size_t>
{
public:
    /// Constructor
    SizeT( size_t initialValue = 0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};


class Double : public PointBasic<double>
{
public:
    /// Constructor
    Double( double initialValue = 0.0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};

class Float : public PointBasic<float>
{
public:
    /// Constructor
    Float( float initialValue = 0.0 );

public:
    /// See Cpl::Rte::Point.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::Point.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};



};      // end namespaces
};
};
#endif  // end header latch
