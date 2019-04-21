#ifndef Cpl_Rte_Peristence_Record_HandlerApi_h_
#define Cpl_Rte_Peristence_Record_HandlerApi_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Dm/Persistence/Record/ApiWriter_.h"


/// Namespace(s)
namespace Cpl {
namespace Dm {
namespace Persistence {
namespace Record {

/** This class defines the interface that individual Records call when interacting
    with the Record Layer's Handler.  All of the methods in this class are intended
    to be call in the same thread as the Record Server, i.e. from the Record Layer
    thread.
 */
class HandlerApi_
{
public:
    /** Informs the handler that the Record is waiting to be loaded/
        initialized from its associated persistent record
     */
    virtual void notifyRecordWaiting( void ) = 0;

    /** Informs the handler that the Record has been succesfully loaded/
        initialized
     */
    virtual void notifyRecordInitialized( void ) = 0;

    /** Informs the handler that the Record has been succesfully loaded/
        initialized - but raw data from the persistent record had to be
        converted (i.e. an upgrade or downgrade occurred on the record)
     */
    virtual void notifyRecordConverted( void ) = 0;

    /** This informs the handler that the record has been succesfully
        stopped.
     */
    virtual void notifyRecordStopped( void ) = 0;

    /** This informs the handler that the record has been succesfully
        started.
     */
    virtual void notifyRecordStarted( void ) = 0;


public:
    /** This method initiates the write action (to non-volitile storage) for
        'recordToWrite'
     */
    virtual void write( ApiWriter_& recordToWrite ) = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~HandlerApi_( void ) {}


};


};      // end namespaces
};
};
};
#endif  // end header latch
