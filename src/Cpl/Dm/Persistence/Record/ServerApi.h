#ifndef Cpl_Rte_Persistence_Record_ServerApi_h_
#define Cpl_Rte_Persistence_Record_ServerApi_h_
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

#include "Cpl/Itc/CloseSync.h"

/// Namespace(s)
namespace Cpl {
namespace Dm {
namespace Persistence {
namespace Record {

/** This symbol defines the maximum size, in bytes, of record name.  Note: the
    length does NOT include a null terminator.
 */
#ifndef OPTION_CPL_DM_PERSISTENCE_RECORD_MAX_RECORD_NAME_LEN
#define OPTION_CPL_DM_PERSISTENCE_RECORD_MAX_RECORD_NAME_LEN   31
#endif

/** This class defines the interface for the Application to interact with a
    Record Server (aka the Record Layer).  The Record Server manages the loading
    of ALL records and processes the write requests of individual records for a
    single instance of the Model Database.

    NOTE: The current health/status of a Record Server is reported via a
          MpServerStatus Model Point (that the application provides when 
          constructing a concrete Record Server).

    NOTE: The application can default the entire Model Database (or a specified
          individual record) by writing an "*" (or record name) to the String 
          Model Point (that the application provides when constructing a 
          concrete Record Server).  The Record server writes a empty string
          to the String MP when it initiates the default action.  The actual 
          state of the Record(s) (and it Model Points) my still be 'in-flux' 
          when the String MP is updated, i.e. 'when' the data is changed and/or 
          persistent media is updated is dependent on the Record instance and 
          the performance of the overall system.. 

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
    /// Constructor
    ServerApi( Cpl::Itc::PostApi& myMailbox ): CloseSync( myMailbox ) {}

    /// Virtual destructor to keep the compiler happy
    virtual ~ServerApi( void ) {}
};


};      // end namespaces
};
};
};
#endif  // end header latch
