#ifndef Cpl_Rte_ModelDatabase_h_
#define Cpl_Rte_ModelDatabase_h_
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

#include "Cpl/Rte/ModelDatabaseApi.h"
#include "Cpl/System/Mutex.h"


///
namespace Cpl {
///
namespace Rte {


/** This concrete class implements a simple Model Database
 */
class ModelDatabase: public ModelDatabaseApi, Cpl::System::Mutex
{
public:
    /// Constructor
    ModelDatabase() {}

public:
    /// Set Cpl::Rte::ModelDatabaseApi
    void lock_() throw() { lock(); };

    /// Set Cpl::Rte::ModelDatabaseApi
    void unlock_() throw() { unlock(); };
};


};      // end namespaces
};
#endif  // end header latch
