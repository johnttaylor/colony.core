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


#include "Float.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Math/real.h"

///
using namespace Cpl::Rte::Point;

///////////////////////////////////////////////////////////////////////////////
Float::Float( float initialValue )
    :PointBasic( initialValue )
{
}

bool Float::isEqual_( const Point& other ) const throw()
{
    float left = *((float*) other.getDataPointer_());
    return Cpl::Math::areFloatsEqual( m_data, left );
}

bool Float::toString( Cpl::Text::String& dst, bool append=false ) const throw()
{
    dst.formatOpt( append, "%g", (double) m_data );
    return true;
}

const char* Float::fromString( const char* src, const char* terminationChars, Cpl::Text::String* errorMsg ) throw()
{
    const char* endptr;
    double      value;

    if ( Cpl::Text::a2d( value, src, terminationChars, &endptr ) )
    {
        m_data = (float) value;
        return endptr;
    }

    if ( errorMsg )
    {
        *errorMsg = "Conversion to a float failed.";
    }
    return 0;
}
