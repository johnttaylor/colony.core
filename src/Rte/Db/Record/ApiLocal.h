#ifndef Rte_Db_Record_ApiLocal_h_
#define Rte_Db_Record_ApiLocal_h_
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

#include "Rte/Db/Record/HandlerLocal.h"
#include "Rte/Db/Chunk/Handle.h"
#include "Cpl/Container/MapItem.h"



/// Namespace(s)
namespace Rte { namespace Db { namespace Record { 


/** This class defines the interface for an individual Record to interact with 
    the Record Handler.  This interface is intended to be implemented by the
    Record Layer.   It is the Record Handler that calls this interface.  All of
    the methods in this  class are intended to be call in the same thread as the
    Record Server, i.e. from  the Record Layer thread.
 */
class ApiLocal: public Cpl::Container::MapItem
{
public:
    /** This method starts/initializes the Record.  A Record can be restartd after
        stop() is called
     */
    virtual void start( HandlerLocal& recordHandler ) throw() = 0;

    /** This method stops the Record.
     */
    virtual void stop() throw() = 0;

    /** This method is used to default the data contents of the Record to its
        default value(s).
     */
    virtual void defaultContent() throw() = 0;


public:
    /** This method is used to inform the Record that the initial
        load process has been successfully completed
     */
    virtual void notifyLoadCompleted(void) = 0;

    /** This method is used to inform the set that the initial
        load process has failed and/or is incomplete.
     */
    virtual void notifyLoadFailed(void) = 0;


public:
    /// This method is used to set the contents of the record's chunk handle
    virtual void setChunkHandle( Rte::Db::Chunk::Handle& src ) = 0;

    /** Provides the notification that the record data has been
        read from the non-volatile media.  The record data is 
        contained in 'srcBuffer'.  The owner of record is
        responsible for copying the content of recData into the
        record's internal storage.  This method return true if
        succesful (i.e the data available matches what the
        record owner is expecting); else false is return.  A result
        of false is treated/handled/acted-upon (by the Record Layer) 
        as an incompatible Database.
     */
    virtual bool notifyRead( void* srcBuffer, uint32_t dataLen ) = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~ApiLocal(void){}
};


};      // end namespaces
};
};
#endif  // end header latch
