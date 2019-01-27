#ifndef Cpl_Rte_Persistence_Chunk_Server_h_
#define Cpl_Rte_Persistence_Chunk_Server_h_
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


#include "Cpl/Checksum/Api32.h"
#include "Cpl/Rte/Persistence/Chunk/Request.h"
#include "Cpl/Rte/Persistence/Chunk/Media.h"


///
namespace Cpl {
namespace Rte {
namespace Persistence {
namespace Chunk {


/** This concrete server implements the Chunk Layer of the RTE Persistent
    storage sub-system. The server is intended to run in its own thread -
    separated from the Record layer.  Note: multiple instances of this server
    can share a single thread
 */
class Server : public Request
{
protected:
    /// Reference to the specific CRC method to be use checksum the chunk data
    Cpl::Checksum::Api32&           m_crc;

    /// Reference to the media storage of my database
    Media&                          m_file;

    /// Signature (aka scheme) identifier of database
    const char*                     m_signature;

    /// Generation number of the database
    uint32_t                        m_generation;

    /// File descriptor of my database file
    Cpl::Io::File::InputOutputApi*  m_fdPtr;


public:
    /// Constructor
    Server( Media& myfile, Cpl::Checksum::Api32& crcApi, const char* fileSignature );

    /// Destructor
    virtual ~Server( void );


public:
    /// See Rte::Db::Chunk::Request
    void request( OpenFileMsg& msg );

    /// See Rte::Db::Chunk::Request
    void request( CloseFileMsg& msg );

    /// See Rte::Db::Chunk::Request
    void request( ClearFileMsg& msg );

    /// See Rte::Db::Chunk::Request
    void request( ReadFileMsg& msg );

    /// See Rte::Db::Chunk::Request
    void request( WriteFileMsg& msg );



protected:
    /// Helper method
    virtual void openFile( ServerResult& result, uint8_t* bufferPtr, uint32_t bufferMaxSize );

    /// Helper method
    virtual void closeFile();

    /// Helper method
    virtual bool writeChunk( ServerResult& result, uint8_t* bufferPtr, uint32_t bufferLen, Handle* handlePtr );

    /// Helper method
    virtual bool writeSignature( ServerResult& result );

    /// Helper method
    virtual bool checkSignature( ServerResult& result, uint8_t* bufferPtr, uint32_t bufferMaxSize );


};

};      // end namespaces
};
};
};
#endif  // end header latch
