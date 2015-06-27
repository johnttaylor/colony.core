#ifndef Rte_Db_Chunk_Request_h_
#define Rte_Db_Chunk_Request_h_
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


#include "Cpl/Itc/RequestMessage.h"
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
    /** This enum defines the actions/operations that can a Chunk Server can 
        perform.
     */
    enum Action_T { eOPENDB,      //!< Opens the database file
                    eCLOSEDB,     //!< Closes the database file
                    eREAD,        //!< Read the next chunk in the opened DB file
                    eWRITE,       //!< Write the specified record/chunk to the opened DB file
                    eCLEARDB,     //!< Delete the contents of the DB file (i.e. re-start with an empty file)
                  };


    /** This enum defines the possible results code for the above 
        operations
     */
    enum Result_T { eSUCCESS,             //!< Request operation succeeded
                    eEOF,                 //!< EOF was reached
                    eCORRUPT_DATA,        //!< The chunk read did not pass its CRC check
                    eERR_FILEIO,          //!< A file operation failed and the operation was aborted
                    eERR_OPEN,        //!< Unable to open the specified DB file
                    eERR_WRONG_FILE       //!< The specified DB file is NOT a DB file
                  };



public:
    /// Payload for Message: Chunk Action
    class ActionPayload
    {
    public:
        /// Chunk Action being request
        Action_T    m_action;

        /// Result/return code for the requested Action
        Result_T    m_result;

        /// The Client's/Caller's buffer to be used in the Action (Client is responsible for providing the Memory)
        uint8_t*    m_buffer;

        /// Maximize size, in bytes, of m_buffer
        uint32_t    m_bufferMaxSize;

        /// The length, in bytes, of the data stored/written to the buffer
        uint32_t    m_bufferLen;

        /// Pointer The Client's/Caller's Chunk Handle (not required for all Chunk actions)
        Handle*     m_handlePtr;
    

    public:
        /// Constructor: For eOPENDB, eCLOSEDB, and eCLEARDB actions
        ActionPayload( Action_T action,
                       uint8_t  buffer,
                       uint32_t bufferMaxSize
                     )
            :m_action(action)
            ,m_result(eSUCCESS)
            ,m_buffer(buffer)
            ,m_bufferMaxSize(bufferMaxSize)
            ,m_bufferLen(0)
            ,m_handlePtr(0)
                {}

        /// Constructor: For eREAD action
        ActionPayload( Action_T action,
                       uint8_t  buffer,
                       uint32_t bufferMaxSize,
                       Handle&  clientChunkHandle
                     )
            :m_action(action)
            ,m_result(eSUCCESS)
            ,m_buffer(buffer)
            ,m_bufferMaxSize(bufferMaxSize)
            ,m_bufferLen(0)
            ,m_handlePtr(&clientChunkHandle)
                {}


        /// Constructor: For eWRITE action
        ActionPayload( Action_T action,
                       Handle&  clientChunkHandle
                       uint8_t  buffer,
                       uint32_t bufferLen,
                     )
            :m_action(action)
            ,m_result(eSUCCESS)
            ,m_buffer(buffer)
            ,m_bufferMaxSize(0)
            ,m_bufferLen(bufferLen)
            ,m_handlePtr(&clientChunkHandle)
                {}

    /// Message Type: Chunk Action
    typedef Cpl::Itc::RequestMessage<Request,ActionPayload> ActionMsg;


public:
    /// Request: Performa a Chunk Action
    virtual void request( ActionMsg& msg ) = 0;
    

public:
    /// Helper method that converts a Action_T enum value to a string constant
    static const char* actionToString( Action_T action ) const throw();

    /// Helper method that converta a Result_T enum value to a string constant
    static const char* resultToString( Result_T result ) const throw();
};




/** This abstact class define message types and payloads for the set of
    ITC RESPONSE services used by Client (Record Layer) to invoke Chunk Actions
 */
class Response
{
public:
    /// Response Message Type: Action response
    typedef Cpl::Itc::ResponseMessage<Response,
                                      Request,
                                      Request::ActionPayload> ActionMsg;
    
            
    /// Response: Action response
    virtual void response( ActionMsg& msg ) = 0;

    
};


};      // end namespaces
};
};
#endif  // end header latch
