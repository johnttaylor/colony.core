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
    Media&                          m_db;

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
    void request( OpenDbMsg& msg );
    
    /// See Rte::Db::Chunk::Request
    void request( CloseDbMsg& msg );
    
    /// See Rte::Db::Chunk::Request
    void request( ClearDbMsg& msg );
    
    /// See Rte::Db::Chunk::Request
    void request( ReadMsg& msg );
    
    /// See Rte::Db::Chunk::Request
    void request( WriteMsg& msg );



protected:
    /// Helper method
    virtual void openDB( Request::Result_T& result, uint8_t* bufferPtr, uint32_t bufferMaxSize );

    /// Helper method
    virtual void closeDB();

    /// Helper method
    virtual bool writeChunk( Request::Result_T& result, uint8_t* bufferPtr, uint32_t bufferLen, Handle* handlePtr );

    /// Helper method
    virtual bool writeSignature( Request::Result_T& result );

    /// Helper method
    virtual bool checkSignature( Request::Result_T& result, uint8_t* bufferPtr, uint32_t bufferMaxSize );


};

};      // end namespaces
};
};
#endif  // end header latch
