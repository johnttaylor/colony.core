#ifndef Rte_Db_Chunk_Server_h_
#define Rte_Db_Chunk_Server_h_
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


#include "Rte/Db/Chunk/Request.h"
#include "Rte/Db/Chunk/Media.h"
#include "Cpl/Checksum/Api32.h"


///
namespace Rte { namespace Db { namespace Chunk {


/** This concrete server implements the Chunk Layer of the Eden Database.
    The server is intended to run in its own thread - seperated from the
    Record layer.  Note: multiple instances of this server can share a
    a single thread
 */
class Server: public Request
{
protected:
    /// Reference to the media storage of my database
    Media                           m_db;

    /// Reference to the specific CRC method to be use checksum the chunk data
    Cpl::Checksum::Api32&           m_crc;

    /// Signature (aka scheme) identifier of database
    const char*                     m_signature;

    /// Generation number of the database
    uint32_t                        m_generation;

    /// File descriptor of my database file
    Cpl::Io::File::InputOutputApi*  m_fdPtr;


public:
    /// Constructor
    Server( Media& dbfile, Cpl::Checksum::Api32& crcApi, const char* dbSignature );

    /// Destructor
    virtual ~Server(void);


public: 
    /// See Rte::Db::Chunk::Request
    void request( ActionMsg& msg );
    

protected:
    /// Helper method that perform the: Open DB action
    virtual void openDB( Request::ActionPayload& action );

    /// Helper method that perform the: Close DB action
    virtual void closeDB();

    /// Helper method that perform the: Clear DB action
    virtual void clearDB( Request::ActionPayload& action );

    /// Helper method that perform the: Read action
    virtual void read( Request::ActionPayload& action );

    /// Helper method that perform the: Write action
    virtual void write( Request::ActionPayload& action );


protected:
    /// Helper method
    virtual bool writeChunk( Request::ActionPayload& action );

    /// Helper method
    virtual bool writeSignature( Request::ActionPayload& action );

    /// Helper method
    virtual bool checkSignature( Request::ActionPayload& action );


};

};      // end namespaces
};
};
#endif  // end header latch
