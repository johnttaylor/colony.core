#ifndef Rte_Db_Chunk_Request_h_
#define Rte_Db_Chunk_Request_h_
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


#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"
#include "Rte/Db/Chunk/Handle.h"
#include <stdint.h>


///
namespace Rte { namespace Db { namespace Chunk {

/** This abstact class define message types and payloads for the set of ITC 
    services used a Chunk Server. A Chunk Server (or Chunk Layer) is
    responsible for checking/providing data  integrity for the Db's
    contents.
 */
class Request
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<Request> SAP;


public:
    /** This enum defines the possible results code for the above 
        operations
     */
    enum Result_T { eSUCCESS,             //!< Request operation succeeded
                    eEOF,                 //!< EOF was reached
                    eCORRUPT_DATA,        //!< The chunk read did not pass its CRC check
                    eWRONG_SCHEMA,        //!< The specified DB file's signature (schema identifier) does not match the expected signature
                    eERR_FILEIO,          //!< A file operation failed and the operation was aborted
                    eERR_OPEN,            //!< Unable to open the specified DB file
                    eERR_NOT_A_DB_FILE    //!< The specified DB file is NOT a DB file
                  };



public:
    /// Payload for Message: Open Database
    class OpenDbPayload
    {
    public:
        /// Result/return code for the requested Action
        Result_T    m_result;

        /// The Client's/Caller's buffer to be used in the Action (Client is responsible for providing the Memory)
        uint8_t*    m_buffer;

        /// Maximize size, in bytes, of m_buffer
        uint32_t    m_bufferMaxSize;
    

    public:
        /// Constructor
        OpenDbPayload( uint8_t* buffer,
                       uint32_t bufferMaxSize
                     )
            :m_result(eSUCCESS)
            ,m_buffer(buffer)
            ,m_bufferMaxSize(bufferMaxSize)
                {}

    };

    /// Message Type: Open Database
    typedef Cpl::Itc::RequestMessage<Request,OpenDbPayload> OpenDbMsg;


public:
    /// Payload for Message: Close Database
    class CloseDbPayload
    {
    public:
        /// Result/return code for the requested Action
        Result_T    m_result;
    
    public:
        /// Constructor
        CloseDbPayload()
            :m_result(eSUCCESS)
                {}
    };

    /// Message Type: Chunk Action
    typedef Cpl::Itc::RequestMessage<Request,CloseDbPayload> CloseDbMsg;



public:
    /// Payload for Message: Clear Database
    class ClearDbPayload
    {
    public:
        /// Result/return code for the requested Action
        Result_T    m_result;

        /// The Client's/Caller's buffer to be used in the Action (Client is responsible for providing the Memory)
        uint8_t*    m_buffer;

        /// Maximize size, in bytes, of m_buffer
        uint32_t    m_bufferMaxSize;
    

    public:
        /// Constructor
        ClearDbPayload( uint8_t* buffer,
                        uint32_t bufferMaxSize
                      )
            :m_result(eSUCCESS)
            ,m_buffer(buffer)
            ,m_bufferMaxSize(bufferMaxSize)
                {}

    };

    /// Message Type: Clear Database
    typedef Cpl::Itc::RequestMessage<Request,ClearDbPayload> ClearDbMsg;




public:
    /// Payload for Message: Read
    class ReadPayload
    {
    public:
        /// Result/return code for the requested Action
        Result_T    m_result;

        /// The Client's/Caller's buffer to be used in the Action (Client is responsible for providing the Memory)
        uint8_t*    m_buffer;

        /// Maximize size, in bytes, of m_buffer
        uint32_t    m_bufferMaxSize;

        /// The length, in bytes, of the data read into the buffer
        uint32_t    m_bufferLen;

        /// Pointer The Client's/Caller's Chunk Handle (not required for all Chunk actions)
        Handle*     m_handlePtr;
    

    public:
        /// Constructor: 
        ReadPayload( uint8_t* buffer,
                     uint32_t bufferMaxSize,
                     Handle&  clientChunkHandle
                   )
            :m_result(eSUCCESS)
            ,m_buffer(buffer)
            ,m_bufferMaxSize(bufferMaxSize)
            ,m_bufferLen(0)
            ,m_handlePtr(&clientChunkHandle)
                {}
    };


    /// Message Type: Read Action
    typedef Cpl::Itc::RequestMessage<Request,ReadPayload> ReadMsg;


public:
    /// Payload for Message: Write
    class WritePayload
    {
    public:
        /// Result/return code for the requested Action
        Result_T    m_result;

        /// The Client's/Caller's buffer to be used in the Action (Client is responsible for providing the Memory)
        uint8_t*    m_buffer;

        /// Maximize size, in bytes, of m_buffer
        uint32_t    m_bufferMaxSize;

        /// The length, in bytes, of the data to be written to the buffer
        uint32_t    m_bufferLen;

        /// Pointer The Client's/Caller's Chunk Handle (not required for all Chunk actions)
        Handle*     m_handlePtr;
    

    public:
        /// Constructor: 
        WritePayload( uint8_t* buffer,
                      uint32_t dataLen,
                      Handle&  clientChunkHandle
                    )
            :m_result(eSUCCESS)
            ,m_buffer(buffer)
            ,m_bufferMaxSize(0)
            ,m_bufferLen(dataLen)
            ,m_handlePtr(&clientChunkHandle)
                {}
    };


    /// Message Type: Write Action
    typedef Cpl::Itc::RequestMessage<Request,WritePayload> WriteMsg;



public:
    /// Request: Open Database
    virtual void request( OpenDbMsg& msg ) = 0;
    
    /// Request: Close Database
    virtual void request( CloseDbMsg& msg ) = 0;
    
    /// Request: Clear Database
    virtual void request( ClearDbMsg& msg ) = 0;
    
    /// Request: Read action
    virtual void request( ReadMsg& msg ) = 0;
    
    /// Request: Write action
    virtual void request( WriteMsg& msg ) = 0;


public:
    /// Helper method that converta a Result_T enum value to a string constant
    static const char* resultToString( Result_T result ) throw();
};




/** This abstact class define message types and payloads for the set of
    ITC RESPONSE services used by Client (Record Layer) to invoke Chunk Actions
 */
class Response
{
public:
    /// Response Message Type: Open Database
    typedef Cpl::Itc::ResponseMessage<Response,
                                      Request,
                                      Request::OpenDbPayload>  OpenDbMsg;
    
    /// Response Message Type: Close Database
    typedef Cpl::Itc::ResponseMessage<Response,
                                      Request,
                                      Request::CloseDbPayload> CloseDbMsg;
    
    /// Response Message Type: Clear Database
    typedef Cpl::Itc::ResponseMessage<Response,
                                      Request,
                                      Request::ClearDbPayload> ClearDbMsg;
    
    /// Response Message Type: Read Action
    typedef Cpl::Itc::ResponseMessage<Response,
                                      Request,
                                      Request::ReadPayload>    ReadMsg;
    
    /// Response Message Type: Write Action
    typedef Cpl::Itc::ResponseMessage<Response,
                                      Request,
                                      Request::WritePayload>   WriteMsg;
    
            


public:
    /// Response: Open Database
    virtual void response( OpenDbMsg& msg ) = 0;

    /// Response: Close Database
    virtual void response( CloseDbMsg& msg ) = 0;
    
    /// Response: Clear Database
    virtual void response( ClearDbMsg& msg ) = 0;
    
    /// Response: Read action
    virtual void response( ReadMsg& msg ) = 0;
    
    /// Response: Write action
    virtual void response( WriteMsg& msg ) = 0;
    
};


};      // end namespaces
};
};
#endif  // end header latch
