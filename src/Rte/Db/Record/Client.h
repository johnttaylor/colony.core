#ifndef Rte_Db_Record_Client_h_
#define Rte_Db_Record_Client_h_
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

#include "Rte/Db/Record/Api.h"
#include <stdint.h>


/// Namespace(s)
namespace Rte { namespace Db { namespace Record { 


/** This class defines the interface for calling back into the upper
    layer's (aka the Set Layer) Handler.  This upper layer handler is
    responsible for supervising the actions of all entities (aka sets).
 */
class Client
{
public:
    /// Possible Open errors
    enum OpenError_T { eCORRUPT_DATA,       //!< At least one Record was corrupted
                       eMISSING_SETS,       //!< The number of Record in the DB was less than expected (aka a minor upgrade)
                       eERR_MEDIA,          //!< A File/Media error was encounter.  The DB is operating in the 'No Persistence' state
                       eERR_WRONG_SCHEMA    //!< The specified DB file does not have a compatible schema.  The DB is operating in the 'No Persistence' state
                     };

public:
    /** Returns the Record API associated with 'recName'.  If no match
        is found for recName, then null is returned.

        Note: 'recName' is NOT a null terminated string - it is an
              unterminated array of characters with an array size
              of nameLen.
     */
    virtual Api* getRecordApi( const char* recName, uint16_t nameLen ) = 0;


    /** Notification (to the upper layer) that all records have been 
        read from non-volatile storage.
     */
    virtual void notifyOpenCompleted(void) = 0;


    /** Notification (to the upper layer) that an error occurred during 
        the LOAD of the records and loading process has been stopped/
        aborted. The data in any of the record(s) succesfully opened are
        still validate at this point.  It is the application's 
        responsibility to decide what actions (if any) it needs to take 
        for the record(s) that loaded succesfully.

        NOTE: The database is in the OPENED state at this point.  For 
              example: for the use case of a new record has been added 
              to application (but the existing DB file does not contain
              the new record) - this method will be called.  At this point
              the application needs to add (i.e. write) the new record to 
              the opened DB.
     */
    virtual void notifyOpenedWithErrors( OpenError_T errorCode ) = 0;

    /** This method allows the upper layer to inform the Record layer
        if there was a clean/succesful load of all the records from
        non-volatile storage.  If this method returns false, then the
        record layer will clear non-volatile storage (so that ALL of
        records can be re-written 'cleanly'). 
     */
    virtual bool isCleanLoad(void) = 0;
    

    /** Notification (to the upper layer) that an error occurred while
        attempting to write a record to the database.  If this happens
        then future write will be attempted and the database operates
        in its 'No Persistence' mode.
     */
    virtual void notifyWriteError(void) = 0;

    
    /** Notification (to the upper layer) that the Record Handler is
        stopped, i.e. the Record Handler/Layer has returned to the 
        Idle state.  This method is the asynchronous response to the 
        Handler::stop() call).
     */
    virtual void notifyStopped() = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~Client(){}
};



};      // end namespaces
};
};
#endif  // end header latch
