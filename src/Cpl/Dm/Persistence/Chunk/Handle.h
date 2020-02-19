#ifndef Cpl_Rte_Persistent_Chunk_Handle_h_
#define Cpl_Rte_Persistent_Chunk_Handle_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include <stdint.h>

///
namespace Cpl {
namespace Dm {
namespace Persistence {
namespace Chunk {


/** This concrete class is data structure that identifies a specific chunk in 
    the Media file
 */
class Handle
{
public:
    /// File offset to the start of the chunk (i.e. ptr to the 'chunk len' field)
    unsigned long   m_offset;

    /// Length of the chunk (does not include Chunk framing fields)
    uint32_t        m_len;

    /** The Handles 'generation'.  A Generation of '0' is used to indicate
        the handle has not be associated with a physical chunk in the database
        file.  Any time, the Chunk server's generation number is greater
        than the handle's generation number, the is handle treated as 'not
        associated'
     */
    uint32_t        m_generation;


public:
    /// Default constructor -->sets the handle to "not associated"
    Handle( void )
        :m_offset( 0 )
        , m_len( 0 )
        , m_generation( 0 )
    {}

};


};      // end namespaces
};
};
};
#endif  // end header latch
