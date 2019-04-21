#ifndef Cpl_Rte_Persistence_Record_ServerStatus_h_
#define Cpl_Rte_Persistence_Record_ServerStatus_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Type/enum.h"


///
namespace Cpl {
namespace Dm {
namespace Persistence {
namespace Record {

/** The following enum/class defines the status of a Record Server.
    \param ServerStatus             - Class name for the Record Server Status enum
    \param int                      - storage type of the enum
    \param eOPENING                 - The Persistent file is in the process of being opened
    \param eRUNNING                 - The Persistent file has been opened and is functional
    \param eRUNNING_MINOR_UPGRADE   - The Persistent file is opened and functional BUT an 'automatic' minor upgrade was performed during the open process
    \param eRUNNING_CORRUPTED_INPUT - The Persistent file is opened and functional BUT one or more records where corrupt and one or more records were defaulted (records successfully read before the corruption was detected are NOT defaulted)
    \param eNO_STORAGE_MEDIA_ERR    - The DB is opened BUT not storing anything to persistence storage because there an error with the media 
    \param eNO_STORAGE_WRONG_SCHEMA - The DB is opened BUT not storing anything to persistence storage because of 'major' schema mismatch
    \param eCLOSING                 - The Persistent is in the process of being closed
    \param eCLOSED                  - The Persistent is in the closed state
    \param eUNKNOWN                 - Unknown status.  This state is typically used by the Monitor for its initial/assumed state
 */
BETTER_ENUM( ServerStatus, int, eOPENING, eRUNNING, eRUNNING_MINOR_UPGRADE, eRUNNING_CORRUPTED_INPUT, eNO_STORAGE_MEDIA_ERR, eNO_STORAGE_WRONG_SCHEMA, eCLOSING, eCLOSED, eUNKNOWN );


};      // end namespaces
};
};
};
#endif  // end header latch
