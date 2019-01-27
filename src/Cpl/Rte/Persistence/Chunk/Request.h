#ifndef Cpl_Rte_Persistence_Chunk_Request_h_
#define Cpl_Rte_Persistence_Chunk_Request_h_
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

#include "Cpl/Rte/Persistence/Chunk/Handle.h"
#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"
#include "Cpl/Type/enum.h"


///
namespace Cpl {
namespace Rte {
namespace Persistence {
namespace Chunk {

/** This enum/class defines the possible results code ITC Request to Chunk Server.
    \param ServerResult             - Class name for the ITC Result codes enum 
    \param int                      - storage type of the enum
    \param eSUCCESS                 - Request operation succeeded
    \param eEOF                     - EOF was reached
    \param eCORRUPT_DATA            - The chunk read did not pass its CRC check
    \param eWRONG_SCHEMA            - The specified Media file's signature (schema identifier) does not match the expected signature
    \param eERR_FILEIO              - A file operation failed and the operation was aborted
    \param eERR_OPEN                - Unable to open the specified Media file
    \param eERR_NOT_A_MEDIA_FILE    - The specified Media file is NOT a Persistent file
 */
BETTER_ENUM( ServerResult, int, eSUCCESS, eEOF, eCORRUPT_DATA, eWRONG_SCHEMA, eERR_FILEIO, eERR_OPEN, eERR_NOT_A_MEDIA_FILE );


/** This abstract class define message types and payloads for the set of ITC
    services used by a Chunk Server. A Chunk Server (or Chunk Layer) is
    responsible for checking/providing data  integrity for the Db's
    contents.
 */
class Request
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<Request> SAP;


public:
    /// Payload for Message: Open Media File
    class OpenFilePayload
    {
    public:
        /// Result/return code for the requested Action
        ServerResult    m_result;

        /// The Client's/Caller's buffer to be used in the Action (Client is responsible for providing the Memory)
        uint8_t*        m_buffer;

        /// Maximize size, in bytes, of m_buffer
        uint32_t        m_bufferMaxSize;


    public:
        /// Constructor
        OpenFilePayload( uint8_t* buffer,
                         uint32_t bufferMaxSize
        )
            :m_result( ServerResult::eSUCCESS )
            , m_buffer( buffer )
            , m_bufferMaxSize( bufferMaxSize )
        {}

    };

    /// Message Type: Open Database
    typedef Cpl::Itc::RequestMessage<Request, OpenFilePayload> OpenFileMsg;


public:
    /// Payload for Message: Open Media File
    class CloseFilePayload
    {
    public:
        /// Result/return code for the requested Action
        ServerResult    m_result;

    public:
        /// Constructor
        CloseFilePayload()
            :m_result( ServerResult::eSUCCESS )
        {}
    };

    /// Message Type: Chunk Action
    typedef Cpl::Itc::RequestMessage<Request, CloseFilePayload> CloseFileMsg;



public:
    /// Payload for Message: Clear Media File (aka delete all storage content)
    class ClearFilePayload
    {
    public:
        /// Result/return code for the requested Action
        ServerResult    m_result;

        /// The Client's/Caller's buffer to be used in the Action (Client is responsible for providing the Memory)
        uint8_t*        m_buffer;

        /// Maximize size, in bytes, of m_buffer
        uint32_t        m_bufferMaxSize;


    public:
        /// Constructor
        ClearFilePayload( uint8_t* buffer,
                          uint32_t bufferMaxSize
        )
            :m_result( ServerResult::eSUCCESS )
            , m_buffer( buffer )
            , m_bufferMaxSize( bufferMaxSize )
        {}

    };

    /// Message Type: Clear Media File
    typedef Cpl::Itc::RequestMessage<Request, ClearFilePayload> ClearFileMsg;




public:
    /// Payload for Message: Read Media File
    class ReadPayload
    {
    public:
        /// Result/return code for the requested Action
        ServerResult    m_result;

        /// The Client's/Caller's buffer to be used in the Action (Client is responsible for providing the Memory)
        uint8_t*        m_buffer;

        /// Maximize size, in bytes, of m_buffer
        uint32_t        m_bufferMaxSize;

        /// The length, in bytes, of the data read into the buffer
        uint32_t        m_bufferLen;

        /// Pointer The Client's/Caller's Chunk Handle (not required for all Chunk actions)
        Handle*         m_handlePtr;


    public:
        /// Constructor: 
        ReadPayload( uint8_t*  buffer,
                     uint32_t  bufferMaxSize,
                     Handle&   clientChunkHandle
        )
            :m_result( ServerResult::eSUCCESS )
            , m_buffer( buffer )
            , m_bufferMaxSize( bufferMaxSize )
            , m_bufferLen( 0 )
            , m_handlePtr( &clientChunkHandle )
        {}
    };


    /// Message Type: Read Action
    typedef Cpl::Itc::RequestMessage<Request, ReadPayload> ReadFileMsg;


public:
    /// Payload for Message: Write Media File
    class WritePayload
    {
    public:
        /// Result/return code for the requested Action
        ServerResult    m_result;

        /// The Client's/Caller's buffer to be used in the Action (Client is responsible for providing the Memory)
        uint8_t*        m_buffer;

        /// Maximize size, in bytes, of m_buffer
        uint32_t        m_bufferMaxSize;

        /// The length, in bytes, of the data to be written to the buffer
        uint32_t        m_bufferLen;

        /// Pointer The Client's/Caller's Chunk Handle (not required for all Chunk actions)
        Handle*         m_handlePtr;


    public:
        /// Constructor: 
        WritePayload( uint8_t*  buffer,
                      uint32_t  dataLen,
                      Handle&   clientChunkHandle
        )
            :m_result( ServerResult::eSUCCESS )
            , m_buffer( buffer )
            , m_bufferMaxSize( 0 )
            , m_bufferLen( dataLen )
            , m_handlePtr( &clientChunkHandle )
        {}
    };


    /// Message Type: Write Action
    typedef Cpl::Itc::RequestMessage<Request, WritePayload> WriteFileMsg;



public:
    /// Request: Open Media File
    virtual void request( OpenFileMsg& msg ) = 0;

    /// Request: Close Media File
    virtual void request( CloseFileMsg& msg ) = 0;

    /// Request: Clear Media File
    virtual void request( ClearFileMsg& msg ) = 0;

    /// Request: Read Media File
    virtual void request( ReadFileMsg& msg ) = 0;

    /// Request: Write Media File
    virtual void request( WriteFileMsg& msg ) = 0;
};




/** This abstract class define message types and payloads for the set of
    ITC RESPONSE services used by Client (Record Layer) to invoke Chunk Actions
 */
class Response
{
public:
    /// Response Message Type: Open Database
    typedef Cpl::Itc::ResponseMessage<Response,
        Request,
        Request::OpenFilePayload>  OpenFileMsg;

    /// Response Message Type: Close Database
    typedef Cpl::Itc::ResponseMessage<Response,
        Request,
        Request::CloseFilePayload> CloseFileMsg;

    /// Response Message Type: Clear Database
    typedef Cpl::Itc::ResponseMessage<Response,
        Request,
        Request::ClearFilePayload> ClearFileMsg;

    /// Response Message Type: Read Action
    typedef Cpl::Itc::ResponseMessage<Response,
        Request,
        Request::ReadPayload>    ReadFileMsg;

    /// Response Message Type: Write Action
    typedef Cpl::Itc::ResponseMessage<Response,
        Request,
        Request::WritePayload>   WriteFileMsg;




public:
    /// Response: Open Database
    virtual void response( OpenFileMsg& msg ) = 0;

    /// Response: Close Database
    virtual void response( CloseFileMsg& msg ) = 0;

    /// Response: Clear Database
    virtual void response( ClearFileMsg& msg ) = 0;

    /// Response: Read action
    virtual void response( ReadFileMsg& msg ) = 0;

    /// Response: Write action
    virtual void response( WriteFileMsg& msg ) = 0;

};


};      // end namespaces
};
};
};
#endif  // end header latch
