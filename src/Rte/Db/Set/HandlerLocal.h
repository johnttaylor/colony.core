#ifndef Rte_Db_Set_HandlerLocal_h_
#define Rte_Db_Set_HandlerLocal_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014, 2015  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Rte/Db/Set/ApiLocal.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Set { 


/** This class defines the interface that individual Sets call when interacting 
    with the Set Layer's Handler.  All of the methods in this class are intended
    to be call in the same thread as the Set Server, i.e. from the Set Layer
    thread. 
 */
class HandlerLocal
{
public:
    /** Informs the handler that the set is waiting to be loaded/
        initialized from its associated DB record
     */
    virtual void notifySetWaiting( ApiLocal& set ) = 0;

    /** Informs the handler that the set has been succesfully loaded/
        initialized
     */
    virtual void notifySetInitialized( ApiLocal& set ) = 0;

    /** Informs the handler that the set has been succesfully loaded/
        initialized - but raw data from the DB's record had to be 
        converted (i.e. an upgrade or downgrade occurred on the set)
     */
    virtual void notifySetConverted( ApiLocal& set ) = 0;

    /** This informs the handler that the set has been succesfully
        stopped.
     */
    virtual void notifySetStopped( ApiLocal& set ) = 0;

    /** This informs the handler that the set has been succesfully
        started.
     */
    virtual void notifySetStarted( ApiLocal& set ) = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~HandlerLocal(void){}


};


};      // end namespaces
};
};
#endif  // end header latch
