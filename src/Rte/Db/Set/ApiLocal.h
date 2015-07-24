#ifndef Rte_Db_Set_ApiLocal_h_
#define Rte_Db_Set_ApiLocal_h_
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

#include "Rte/Db/Record/Handler.h"
#include "Cpl/Container/MapItem.h"



/// Namespace(s)
namespace Rte { namespace Db { namespace Set { 


/** This class defines the interface for an individual Set.  This interface is 
    intended to implemented by the Application.  It is the Set Handler and/or 
    Set Layer the calls this interface.  All of the methods in this class are 
    intended to be call in the same thread as the Set Server, i.e. from the Set 
    Layer thread.
 */
class ApiLocal: public Cpl::Container::MapItem
{
public:
    /** This method starts/initializes the Set.  A Set can be restartd after
        stop() is called
     */
    virtual void start( Rte::Db::Record::Handler& recordLayer ) throw() = 0;

    /** This method stops the Set.
     */
    virtual void stop() throw() = 0;

    /** This method is used to inform the Set that the initial
        load process has been successfully completed
     */
    virtual void notifyLoadCompleted(void) = 0;

    /** This method is used to inform the set that the initial
        load process has failed and/or is incomplete.
     */
    virtual void notifyLoadFailed(void) = 0;
    
    /** This method is used to default the data contents of the Set to its
        default value(s).
     */
    virtual void defaultContent() throw() = 0;

    /** This method returns the Record assocaited with this Set
     */
    virtual Rte::Db::Record::Api& getRecord() throw() = 0;
     

public:
    /// Virtual destructor to keep the compiler happy
    virtual ~ApiLocal(void){}
};


};      // end namespaces
};
};
#endif  // end header latch
