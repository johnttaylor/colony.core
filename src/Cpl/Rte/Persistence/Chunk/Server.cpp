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


#include "Server.h"
#include "Cpl/Rte/Persistence/Framing.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
//#include <string.h>

////////
using namespace Cpl::Rte::Persistence::Chunk;

#define SECT_   "Cpl::Rte::Persistence::Chunk"

//////////////////////////////////
Server::Server( Cpl::Rte::Persistence::Chunk::Media& myFile, Cpl::Checksum::Api32& crcApi, const char* fileSignature )
    :m_crc( crcApi )
    , m_file( myFile )
    , m_signature( fileSignature )
    , m_generation( 0 )
    , m_fdPtr( 0 )
{
}


Server::~Server( void )
{
    closeFile();
}


//////////////////////////////////
void Server::request( OpenFileMsg& msg )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request( OpenFileMsg )") );

    Request::OpenFilePayload& operation = msg.getPayload();
    openFile( operation.m_result, operation.m_buffer, operation.m_bufferMaxSize );

    msg.returnToSender();
}


void Server::request( CloseFileMsg& msg )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request( CloseFileMsg )") );

    closeFile();

    msg.returnToSender();
}


void Server::request( ClearFileMsg& msg )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request( ClearFileMsg )") );
    Request::ClearFilePayload& operation = msg.getPayload();

    // Remember my current open/close state
    void* alreadyOpened = m_fdPtr;

    // Make sure the media file is closed
    closeFile();

    // Delete the db file
    if ( !m_file.deleteFile() )
    {
        operation.m_result = ServerResult::eERR_FILEIO;
    }

    else
    {
        // Re-open the file if it was previously opened
        if ( alreadyOpened )
        {
            openFile( operation.m_result, operation.m_buffer, operation.m_bufferMaxSize );
        }
    }

    msg.returnToSender();
}


//////////////////////////////////
void Server::request( ReadFileMsg& msg )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request( ReadFileMsg )") );
    Request::ReadPayload& operation = msg.getPayload();

    // Trap improper usage
    if ( !m_fdPtr )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Chunk::Server::request( ReadMsg ) - media file NOT opened." );
    }

    // Remember the start of the chunk in the file
    operation.m_handlePtr->m_offset = m_fdPtr->currentPos();

    // Get the chunk length
    int bytesRead = 0;
    if ( !m_fdPtr->read( &(operation.m_handlePtr->m_len), Cpl::Rte::Persistence::eCLEN_SIZE, bytesRead ) || (uint32_t) bytesRead != Cpl::Rte::Persistence::eCLEN_SIZE )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request( ReadFileMsg ) - eEOF - failed to read Chunk Length (bytesRead=%d)", bytesRead) );
        operation.m_result = ServerResult::eEOF;
        msg.returnToSender();
        return;
    }
    if ( operation.m_handlePtr->m_len > operation.m_bufferMaxSize - Rte::Db::eCCRC_SIZE )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request( ReadMsg ) - eERR_FILEIO -  Chunk Length greater than buffer (clen=%u, bufSize=%u)", operation.m_handlePtr->m_len, operation.m_bufferMaxSize - Rte::Db::eCCRC_SIZE) );
        operation.m_result = eERR_NOT_A_MEDIA_FILE;
        msg.returnToSender();
        return;
    }

    // Get the chunk data and the CRC
    if ( !m_fdPtr->read( operation.m_buffer, operation.m_handlePtr->m_len + Rte::Db::eCCRC_SIZE, bytesRead ) || (uint32_t) bytesRead != operation.m_handlePtr->m_len + Rte::Db::eCCRC_SIZE )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request( ReadMsg ) - eERR_FILEIO -  Failed on read of data+crc (bytesRead=%d)", bytesRead) );
        operation.m_result = eERR_FILEIO;
        msg.returnToSender();
        return;
    }

    // Check the CRC of the chunk
    m_crc.reset();
    m_crc.accumulate( &(operation.m_handlePtr->m_len), Rte::Db::eCLEN_SIZE );
    m_crc.accumulate( operation.m_buffer, operation.m_handlePtr->m_len + Rte::Db::eCCRC_SIZE );
    if ( !m_crc.isOkay() )
    {
        operation.m_result = eCORRUPT_DATA;
        msg.returnToSender();
        return;
    }

    // If I get here - the chunk was succesfully read.
    // Mark the chunk handle as "associated" with a chunk in the db file
    operation.m_handlePtr->m_generation = m_generation;

    // Trap if this is the last chunk in the file
    if ( m_fdPtr->isEof() )
    {
        operation.m_result = eEOF;
    }

    msg.returnToSender();
}


void Server::request( WriteMsg& msg )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request( WriteMsg )") );
    Request::WritePayload& operation = msg.getPayload();

    // Trap improper usage
    if ( !m_fdPtr )
    {
        Cpl::System::FatalError::logf( "Rte::Db::Chunk::Server::request( WriteMsg ) - db file NOT opened." );
    }

    // Check if handle is of the correct DB generation
    if ( operation.m_handlePtr->m_generation == m_generation )
    {
        m_fdPtr->setAbsolutePos( operation.m_handlePtr->m_offset );
        writeChunk( operation.m_result, operation.m_buffer, operation.m_bufferLen, operation.m_handlePtr );
    }

    // Handle is stale
    else
    {
        // Append chunk to the end of the file
        m_fdPtr->setToEof();
        operation.m_handlePtr->m_offset = m_fdPtr->currentPos();
        operation.m_handlePtr->m_len    = operation.m_bufferLen;
        if ( writeChunk( operation.m_result, operation.m_buffer, operation.m_bufferLen, operation.m_handlePtr ) )
        {
            // Succesful in adding the chunk to the db file -->mark the handle as "associated"
            operation.m_handlePtr->m_generation = m_generation;
        }
    }

    msg.returnToSender();
}


//////////////////////////////////
void Server::openDB( Request::Result_T& result, uint8_t* bufferPtr, uint32_t bufferMaxSize )
{
    // Nothing to do if already opened
    if ( m_fdPtr )
    {
        return;
    }

    // Open the database file
    bool newfile  = false;
    m_fdPtr       = m_file.openDatabase( newfile );
    if ( !m_fdPtr )
    {
        // Failed to open -->return an error
        result = eERR_OPEN;
        return;
    }

    // Increment the generation number every time the DB is opened
    if ( ++m_generation == 0 )
    {
        m_generation = 1;   // The Chunk server's generation number is required to never be zero
    }


    // Sign the file when it is new
    if ( newfile )
    {
        if ( !writeSignature( result ) )   // Note: the writeSignature() method sets the operation's result code based on its success/failure
        {
            // Failed to write the signature -->close the file 
            closeDB();
        }

        // Signed succsefully -->but Report empty file since there are no data chunks.
        else
        {
            result = eEOF;
        }
    }

    // Existing db file: Verify Signature
    else if ( !checkSignature( result, bufferPtr, bufferMaxSize ) )  // Note: the checkSignature() method sets the operation's result code based on its success/failure
    {
        // Close the file
        closeDB();
    }
}


void Server::closeDB()
{
    // NOTE: Do NOT Put any TRACE statement in this method.  This method is 
    //       call by the destructor and if the server was statically
    //       created - the TRACE method will fail because it uses a statically
    //       allocated mutex -->and there IS NOT order on how static 
    //       destructors are called, i.e. your program will crash (when running
    //       on OS that dynamically runs executables).

    // Nothing to do if already closed
    if ( m_fdPtr )
    {
        m_file.closeDatabase();
        m_fdPtr = 0;
    }
}


bool Server::writeChunk( Request::Result_T& result, uint8_t* bufferPtr, uint32_t bufferLen, Handle* handlePtr )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::writeChunk") );

    // Write Chunk Length
    if ( !m_fdPtr->write( &(handlePtr->m_len), Rte::Db::eCLEN_SIZE ) )
    {
        result = eERR_FILEIO;
        return false;
    }

    // Write Chunk Data
    if ( !m_fdPtr->write( bufferPtr, bufferLen ) )
    {
        result = eERR_FILEIO;
        return false;
    }

    // Calc CRC
    uint32_t crc;
    m_crc.reset();
    m_crc.accumulate( &(handlePtr->m_len), Rte::Db::eCLEN_SIZE );
    m_crc.accumulate( bufferPtr, bufferLen );
    m_crc.finalize( &crc );

    // Writ the CRC
    if ( !m_fdPtr->write( &crc, Rte::Db::eCCRC_SIZE ) )
    {
        result = eERR_FILEIO;
        return false;
    }

    return true;
}

bool Server::writeSignature( Request::Result_T& result )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::writeSignature") );

    // Calculate chunk length for the signature
    uint16_t nlen = (uint16_t) strlen( m_signature );
    uint32_t clen = Rte::Db::eNLEN_SIZE + nlen;

    // Write Chunk Length
    m_fdPtr->setAbsolutePos( 0 );
    if ( !m_fdPtr->write( &clen, Rte::Db::eCLEN_SIZE ) )
    {
        result = eERR_FILEIO;
        return false;
    }

    // Write Chunk Data
    if ( !m_fdPtr->write( &nlen, Rte::Db::eNLEN_SIZE ) )
    {
        result = eERR_FILEIO;
        return false;
    }
    if ( !m_fdPtr->write( m_signature, nlen ) )
    {
        result = eERR_FILEIO;
        return false;
    }

    // Calculate the CRC
    uint32_t crc = 0;
    m_crc.reset();
    m_crc.accumulate( &clen, Rte::Db::eCLEN_SIZE );
    m_crc.accumulate( &nlen, Rte::Db::eNLEN_SIZE );
    m_crc.accumulate( (void*) m_signature, nlen );
    m_crc.finalize( &crc );

    // Write the CRC
    if ( !m_fdPtr->write( &crc, Rte::Db::eCCRC_SIZE ) )
    {
        result = eCORRUPT_DATA;
        return false;
    }

    return true;
}

bool Server::checkSignature( Request::Result_T& result, uint8_t* bufferPtr, uint32_t bufferMaxSize )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::checkSignature") );

    // Get the chunk length
    uint32_t clen      = 0;
    int      bytesRead = 0;
    m_fdPtr->setAbsolutePos( 0 );
    if ( !m_fdPtr->read( &clen, Rte::Db::eCLEN_SIZE, bytesRead ) || bytesRead != Rte::Db::eCLEN_SIZE )
    {
        result = eERR_FILEIO;
        return false;
    }
    if ( clen > bufferMaxSize - Rte::Db::eCCRC_SIZE )
    {
        result = eERR_NOT_A_MEDIA_FILE;
        return false;
    }

    // Get the chunk data and the CRC
    if ( !m_fdPtr->read( bufferPtr, clen + Rte::Db::eCCRC_SIZE, bytesRead ) || (uint32_t) bytesRead != clen + Rte::Db::eCCRC_SIZE )
    {
        result = eERR_FILEIO;
        return false;
    }

    // Check the CRC of the chunk
    m_crc.reset();
    m_crc.accumulate( &clen, Rte::Db::eCLEN_SIZE );
    m_crc.accumulate( bufferPtr, clen + Rte::Db::eCCRC_SIZE );
    if ( !m_crc.isOkay() )
    {
        result = eERR_NOT_A_MEDIA_FILE;
        return false;
    }

    // Validate length fields before comparing the signature
    uint16_t nlen = 0;
    memcpy( &nlen, bufferPtr, Rte::Db::eNLEN_SIZE );
    if ( clen != (uint32_t) (nlen + Rte::Db::eNLEN_SIZE) || nlen != strlen( m_signature ) )
    {
        result = eWRONG_SCHEMA;
        return false;
    }

    // Check the signature
    if ( strncmp( (const char*) (bufferPtr + Rte::Db::eNLEN_SIZE), m_signature, nlen ) != 0 )
    {
        result = eWRONG_SCHEMA;
        return false;
    }

    // Trap if this is the last chunk in the file
    if ( m_fdPtr->isEof() )
    {
        result = eEOF;
    }

    return true;
}

