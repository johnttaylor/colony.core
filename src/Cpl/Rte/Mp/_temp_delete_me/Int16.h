#ifndef Cpl_Rte_Point_Int16_h_
#define Cpl_Rte_Point_Int16_h_
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


#include "Cpl/Rte/Point/Basic.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Rte {
///
namespace Point {


/** This class provides a concrete implementation for a Point who's data is a 
    int16_t.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
           documented otherwise.
 */
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



};      // end namespaces
};
};
#endif  // end header latch
