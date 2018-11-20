#ifndef Cpl_Rte_Persistence_Record_ServerApi_h_
#define Cpl_Rte_Persistence_Record_ServerApi_h_
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

#include "Cpl/Itc/CloseSync.h"

/// Namespace(s)
namespace Cpl {
namespace Rte {
namespace Persistence {
namespace Record {


/** This class defines the interface for the Application to interact with a
    Record Server (aka the Record Layer).  The Record Server manages the loading
    of ALL records and processes the write requests of individual records for a
    single instance of the Model Database.

    NOTE: The current health/status of a Record Server is reported via a
          MpServerStatus Model Point (that the application provides when 
          constructing a concrete Record Server).

    NOTE: The method(s) in this class are synchronous wrapper to ITC message
          requests, i.e. the method(s) can ONLY be called from Application
          thread(s) where the Record Server is NOT executing in.

    NOTE: The ITC open request will return false if the Persistent state of
          the Model Database ends up in the 'No Persistence' state during the
          opening process.
 */
class ServerApi : public Cpl::Itc::CloseSync
{
public:
    /** This method is used to default the data contents of ALL Records to their
        default value(s).
     */
    virtual void defaultAllRecordsContent() throw() = 0;

    /** This method is used to default a single Record.  The method returns
        true if the specified record name was found and that it's defaultContent()
        method was called.  Note: The actual state of the Record (and it Model
        Points) my still be 'in-flux' when this method returns, i.e. 'when' the
        data is changed and/or persistent media is updated is dependent on
        the Record instance and the performance of the overall system.
     */
    virtual bool defaultRecord( const char* recordNameToDefault ) throw() = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~ServerApi( void ) {}


};


};      // end namespaces
};
};
};
#endif  // end header latch
