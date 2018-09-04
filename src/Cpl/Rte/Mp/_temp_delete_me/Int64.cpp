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


#include "Int64.h"
#include "Cpl/Text/atob.h"

///
using namespace Cpl::Rte::Point;

///////////////////////////////////////////////////////////////////////////////
Int64::Int64( int64_t initialValue )
    :PointBasic( initialValue ) 
{
}


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
