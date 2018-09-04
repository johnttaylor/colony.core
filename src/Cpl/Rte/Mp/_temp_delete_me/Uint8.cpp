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


#include "Uint8.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Rte::Point;

///////////////////////////////////////////////////////////////////////////////
Uint8::Uint8( uint8_t initialValue )
    :PointBasic( initialValue ) 
{
}

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
