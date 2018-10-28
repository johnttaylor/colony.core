#ifndef Cpl_Rte_Persistence_Record_ServerStatus_h_
#define Cpl_Rte_Persistence_Record_ServerStatus_h_
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


#include "Cpl/Type/enum.h"


///
namespace Cpl { namespace Rte { namespace Persistence { namespace Record {


/** The following enum/class defines the status of a Record Server.
 */
BETTER_ENUM( ServerStatus, int, 
             eOPENING,                   /**!< The DB is in the process of being opened */
             eRUNNING,                   /**!< The DB is opened and functional */
             eRUNNING_MINOR_UPGRADE,     /**!< The DB is opened and functional BUT an 'automatic' minor upgrade was performed during the open process */
             eRUNNING_CORRUPTED_INPUT,   /**!< The DB is opened and functional BUT one or more records where corrupt and one or more records were defaulted (records successfully read before the corruption was detected are NOT defaulted) */
             eNO_STORAGE_MEDIA_ERR,      /**!< The DB is opened BUT not storing anything to persistence storage because there an error with the media  */
             eNO_STORAGE_WRONG_SCHEMA,   /**!< The DB is opened BUT not storing anything to persistence storage because of 'major' schema mismatch */
             eCLOSING,                   /**!< The DB is in the process of being closed */
             eCLOSED,                    /**!< The DB is in the closed state. */
             eUNKNOWN                    /**!< Unknown status.  This state is typically used by the Monitor for its initial/assumed state */
);


};      // end namespaces
};
};
};
#endif  // end header latch
