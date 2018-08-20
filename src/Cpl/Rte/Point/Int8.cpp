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


#include "Int8.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Rte::Point;

///////////////////////////////////////////////////////////////////////////////
Int8::Int8( int8_t initialValue, bool hexStringFormat )
    :PointBasic( initialValue ),
    m_hexFormat(hexStringFormat)
{
}

bool Int8::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    const char* format = m_hexFormat ? "%x": "%d";
    dst.formatOpt( append, format, (int) m_data );
    return true;
}

const char* Int8::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char* endptr;
    int         value;
    if ( Cpl::Text::a2i( value, src, m_hexFormat? 16: 10, terminationChars, &endptr ) )
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

