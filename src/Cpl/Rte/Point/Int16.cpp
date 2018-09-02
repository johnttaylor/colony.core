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


#include "Int16.h"
#include <limits.h>
#include "Cpl/Text/atob.h"

///
using namespace Cpl::Rte::Point;


///////////////////////////////////////////////////////////////////////////////
Int16::Int16( int16_t initialValue )
    :PointBasic( initialValue )
{
}

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

