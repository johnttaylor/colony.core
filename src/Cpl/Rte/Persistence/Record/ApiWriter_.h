#ifndef Cpl_Rte_Persistence_Record_ApiWriter_h_
#define Cpl_Rte_Persistence_Record_ApiWriter_h_
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

#include "Cpl/Container/Item.h"
#include "Cpl/Rte/Persistence/Chunk/Handle.h"



/// Namespace(s)
namespace Cpl {
namespace Rte {
namespace Persistence {
namespace Record {


/** This - private to the namespace - class defines the interface for writing 
    the contents of a Record in RAM to non-volitile storage. It is the Record 
    Handler that calls this interface.  All of the methods in this class are 
    intended to be call in the same thread as the Record Server, i.e. from the 
    Record Layer thread.
 */
class ApiWriter_ : public Cpl::Container::Item
{
public:
    /// Returns the name of the record
    virtual const char* getName( void ) const = 0;

    /// Returns the record's chunk handle
    virtual Cpl::Rte::Persistence::Chunk::Handle& getChunkHandle( void ) = 0;

    /** This method is a request to the owner of the record to copy the
        record's internal storage into dstBuffer.  On return from this
        method, the Record layer will complete the process of writing the
        contents in recData to non-volatile media.  The amount of data
        (in bytes) can not exceed the 'maxDataSize' parameter.  The method
        returns the number of bytes in data copied into dstBuffer.
     */
    virtual uint32_t fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize ) = 0;

    /** Provides a notification that the last write action has completed
        successful or unsuccessfully.  No success/fail information is provided
        since there is no action needed (or could be taken) by an individiual
        Record if the write failed.
     */
    virtual void notifyWriteDone( void ) = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~ApiWriter_( void ) {}


};


};      // end namespaces
};
};
};
#endif  // end header latch
