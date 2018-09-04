#ifndef Cpl_Rte_Mp_Uint32_h_
#define Cpl_Rte_Mp_Uint32_h_
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


#include "Cpl/Rte/Mp/Basic.h"

///
namespace Cpl {
///
namespace Rte {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a 
    uint32_t.
 */
class Uint32 : public Basic<uint32_t>
{
protected:
    /// Flag for to/from string() method
    bool m_decimal;

public:
    /// Constructor
    Uint32( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo* staticInfo, bool decimalFormat=true, uint32_t initialValue = 0, int8_t validState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID );

public:
    /// See Cpl::Rte::ModelPoint.
    bool toString( Cpl::Text::String& dst, bool append=false ) const throw();

    /// See Cpl::Rte::ModelPoint.
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw();
};



};      // end namespaces
};
};
#endif  // end header latch
