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

#include "Rte/Db/Record/Api.h"
#include <stdint.h>


/// Namespace(s)
namespace Rte { namespace Db { namespace Record { 


/** This class defines the interface for Record Handler Interface.  The Record
    Handler manages the loading of ALL records and processes the write requests
    of individual records.
 */
class Handler
{
public:
    /** This class defines the interface for calling back into the upper
        layer's (aka the Set Layer) Handler.  This upper layer handler is
        responsible for supervising the actions of all entities (aka sets).
     */
    class Client
    {
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
        virtual void notifyOpenedWithErrors(void) = 0;

    
        /** Notification (to the upper layer) that the DB file that was open
            does NOT have a compatible schema identifier.  The Chunk layer will 
            internally request to the close the DB when this happens. The upper 
            layer will receive a notifyStopped() call once the DB has been 
            succesfully closed.
         */
        virtual void notifyIncompatible(void) = 0;


        /** This method allows the upper layer to inform the Record layer
            if there was a clean/succesful load of all the records from
            non-volatile storage.  If this method returns false, then the
            record layer will clear non-volatile storage (so that ALL of
            records can be re-written 'cleanly'). 
         */
        virtual bool isCleanLoad(void) = 0;
        

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


public:
    /// Starts the record layer handler
    virtual void start(void) = 0;

    /// Stops the record layer handler
    virtual void stop() = 0;


public:
    /// Initiate the write action (to non-volatile storage) for 'recordToWrite'
    virtual void write( Api& recordToWrite ) = 0;

     
public:
    /// Virtual destructor to keep the compiler happy
    virtual ~Handler(void){}


};


};      // end namespaces
};
};
#endif  // end header latch
