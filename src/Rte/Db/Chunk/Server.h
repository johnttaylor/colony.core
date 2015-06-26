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
    ///
    Cpl::Checksum::Api32&            _crcApi;
    ///
    const char*                 _dbFname;
    ///
    const char*                 _dbSignature;
    ///
    uint32_t                    _generation;
    ///
    Jcl::StdInputOutputFile*    _fdPtr;
    ///
    Jcl::AlignedClass<Jcl::StdInputOutputFile>  _memFd;

public:
    /// Constructor
    Server( Cpl::Checksum::Api32& crcApi, const char* dbFname, const char* dbSignature );

    /// Destructor
    virtual ~Server(void);

public: 
    /// See Rte::Db::Chunk::Request
    void request( ActionMsg& msg );
    
protected:
    ///
    virtual void openDB( Request::ActionPayload& action );
    ///
    virtual void closeDB( Request::ActionPayload& action );
    ///
    virtual void clearDB( Request::ActionPayload& action );
    ///
    virtual void read( Request::ActionPayload& action );
    ///
    virtual void write( Request::ActionPayload& action );

    ///
    virtual bool writeChunk( Request::ActionPayload& action );
    ///
    virtual bool writeSignature( Request::ActionPayload& action );
    ///
    virtual bool checkSignature( Request::ActionPayload& action );


};

};      // end namespaces
};
};
#endif  // end header latch
