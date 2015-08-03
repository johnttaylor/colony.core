#ifndef Rte_Db_Record_Handler_h_
#define Rte_Db_Record_Handler_h_
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

#include "Rte/Db/Record/HealthRequest.h"
#include "Cpl/Itc/CloseSync.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Record { 


/** This class defines the interface for the Application to interact with the 
    Record Handler Interface.  The Record Handler manages the loading of ALL 
    records and processes the write requests of individual records.  Note: The 
    method(s) in class are synchronous wrapper to ITC message requests, i.e. the 
    method(s) can be called from Application threads.

    NOTE: The ITC open request will return false if the DB ends up in the 
          'No Persistence' state during the opening process.
 */
class Handler: public Cpl::Itc::CloseSync
{       
public:
    /// Constructor
    Handler( Cpl::Itc::PostApi& myMbox );

public:
    /// Returns the SAP to the Record/DB Health stats ITC interface
    virtual HealthRequest::SAP&  getHealthSAP(void) = 0;
    

public:
    /** This method is used to default the data contents of ALL Records to their
        default value(s).
     */
    virtual void defaultAllRecordsContent() throw() = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~Handler(void){}


};


};      // end namespaces
};
};
#endif  // end header latch
