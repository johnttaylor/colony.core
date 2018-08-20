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


#include "Basic.h"
#include <limits.h>

///
using namespace Cpl::Rte::Point;

///////////////////////////////////////////////////////////////////////////////
Uint8::Uint8( uint8_t initialValue ):PointBasic( initialValue ) {}

bool Uint8::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%u", (unsigned) m_data );
    return true;
}

const char* Uint8::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char* endptr;
    unsigned    value;
    if ( Cpl::Text::a2ui( value, src, 10, terminationChars, &endptr ) )
    {
        if ( value <= UCHAR_MAX )
        {
            m_data = (uint8_t) value;
            return endptr;
        }
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a uint8_t failed (non-numeric or value too large).";
    }
    return 0;
}

Int8::Int8( int8_t initialValue ):PointBasic( initialValue ) {}

bool Int8::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%d", (int) m_data );
    return true;
}

const char* Int8::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char* endptr;
    int         value;
    if ( Cpl::Text::a2i( value, src, 10, terminationChars, &endptr ) )
    {
        if ( value >= -SCHAR_MIN && value <= SCHAR_MAX )
        {
            m_data = (int8_t) value;
            return endptr;
        }
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a int8_t failed (non-numeric or value too large).";
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
Uint16::Uint16( uint16_t initialValue ):PointBasic( initialValue ) {}

bool Uint16::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%u", (unsigned) m_data );
    return true;
}

const char* Uint16::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char*   endptr;
    unsigned long value;
    if ( Cpl::Text::a2ul( value, src, 10, terminationChars, &endptr ) )
    {
        if ( value <= USHRT_MAX )
        {
            m_data = (uint16_t) value;
            return endptr;
        }
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a uint16_t failed (non-numeric or value too large).";
    }
    return 0;
}

Int16::Int16( int16_t initialValue ):PointBasic( initialValue ) {}

bool Int16::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%d", (int) m_data );
    return true;
}

const char* Int16::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char* endptr;
    long        value;
    if ( Cpl::Text::a2l( value, src, 10, terminationChars, &endptr ) )
    {
        if ( value >= SHRT_MIN && value <= SHRT_MAX )
        {
            m_data = (int16_t) value;
            return endptr;
        }
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a int16_t failed (non-numeric or value too large).";
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
Uint32::Uint32( uint32_t initialValue ):PointBasic( initialValue ) {}

bool Uint32::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%lu", (unsigned long) m_data );
    return true;
}

const char* Uint32::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char*   endptr;
    unsigned long value;
    if ( Cpl::Text::a2ul( value, src, 10, terminationChars, &endptr ) )
    {
        m_data = (uint32_t) value;
        return endptr;
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a uint32_t failed.";
    }
    return 0;
}

Int32::Int32( int32_t initialValue ):PointBasic( initialValue ) {}

bool Int32::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%ld", (long) m_data );
    return true;
}

const char* Int32::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char* endptr;
    long        value;
    if ( Cpl::Text::a2l( value, src, 10, terminationChars, &endptr ) )
    {
            m_data = (int32_t) value;
            return endptr;
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a int32_t failed.";
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
Uint64::Uint64( uint64_t initialValue ):PointBasic( initialValue ) {}

bool Uint64::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%llu", (unsigned long long) m_data );
    return true;
}

const char* Uint64::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char*        endptr;
    unsigned long long value;
    if ( Cpl::Text::a2ull( value, src, 10, terminationChars, &endptr ) )
    {
        m_data = (uint64_t) value;
        return endptr;
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a uint64_t failed.";
    }
    return 0;
}

Int64::Int64( int64_t initialValue ):PointBasic( initialValue ) {}

bool Int64::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%lld", (long long) m_data );
    return true;
}

const char* Int64::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char* endptr;
    long long   value;
    if ( Cpl::Text::a2ll( value, src, 10, terminationChars, &endptr ) )
    {
            m_data = (int64_t) value;
            return endptr;
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a int64_t failed.";
    }
    return 0;
}
