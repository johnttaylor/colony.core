#ifndef Rte_Db_Set_Handler_h_
#define Rte_Db_Set_Handler_h_
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

#include "Rte/Db/Set/HealthRequest.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Set { 


/** This class is used by the Application to force ALL Sets to update their 
    data content (aka the Point data) to its default value. Note: The method(s) in
    class are synchronous wrapper to ITC message requests, i.e. the method(s)
    can be called from Application threads.
 */
class Handler
{        
public:
    /// Returns the SAP to the Set/DB Health stats ITC interface
    virtual HealthRequest::SAP&  getHealthSAP(void) = 0;
    

public:
    /** This method is used to default the data contents of ALL Set to their
        default value(s).
     */
    virtual void defaultAllSetsContent() throw() = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~Handler(void){}
};


};      // end namespaces
};
};
#endif  // end header latch
