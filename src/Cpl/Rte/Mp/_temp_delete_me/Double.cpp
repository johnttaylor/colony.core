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


#include "Double.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Math/real.h"

///
using namespace Cpl::Rte::Point;

///////////////////////////////////////////////////////////////////////////////
Double::Double( double initialValue )
    :PointBasic( initialValue )
{
}

bool Double::isEqual_( const Point& other ) const throw()
{
    double left = *((float*) other.getDataPointer_());
    return Cpl::Math::areDoublesEqual( m_data, left );
}

bool Double::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%g", m_data );
    return true;
}

const char* Double::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char* endptr;
    double      value;

    if ( Cpl::Text::a2d( value, src, terminationChars, &endptr ) )
    {
        m_data = value;
        return endptr;
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a double failed.";
    }
    return 0;
}
