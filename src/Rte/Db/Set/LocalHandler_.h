#ifndef Rte_Db_Set_LocalHandler_x_h_
#define Rte_Db_Set_LocalHandler_x_h_
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

#include "Rte/Db/Set/LocalApi_.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Set { 


/** This private to the namespace class defines the interface that individual 
    Sets call when interacting with the Set Layer's Handler
 */
class LocalHandler_
{
public:
    /** Informs the handler that the set is waiting to be loaded/
        initialized from its associated DB record
     */
    virtual void notifySetWaiting( LocalApi_& set ) = 0;

    /** Informs the handler that the set has been succesfully loaded/
        initialized
     */
    virtual void notifySetInitialized( LocalApi_& set ) = 0;

    /** Informs the handler that the set has been succesfully loaded/
        initialized - but raw data from the DB's record had to be 
        converted (i.e. an upgrade or downgrade occurred on the set)
     */
    virtual void notifySetConverted( LocalApi_& set ) = 0;

    /** This informs the handler that the set has been succesfully
        stopped.
     */
    virtual void notifySetStopped( LocalApi_& set ) = 0;

    /** This informs the handler that the set has been succesfully
        started.
     */
    virtual void notifySetStarted( LocalApi_& set ) = 0;


public:
    /** This method is used to register a Set with the Set Layer
        NOTE: ALL Sets should be register BEFORE the Set Layer is
              started (i.e. before open() is called on the set
              layer).
     */
    virtual void registerSet( LocalApi_& set ) = 0;

     
public:
    /// Virtual destructor to keep the compiler happy
    virtual ~LocalHandler_(void){}


};


};      // end namespaces
};
};
#endif  // end header latch
