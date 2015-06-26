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


#include "Server.h"
#include "eden/db/framing.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include <new>
#include <string.h>

////////
using namespace Rte::Db::Chunk;

#define SECT_   "Rte::Db::Chunk"

//////////////////////////////////
Server::Server( Cpl::Checksum::Api32& crcApi, const char* dbFname, const char* dbSignature )
:_crcApi(crcApi),
 _dbFname(dbFname),
 _dbSignature(dbSignature),
 _generation(0),
 _fdPtr(0)
    {
    }

Server::~Server(void)
    {
    if ( _fdPtr )
        {
        _fdPtr->~StdInputOutputFile();
        _fdPtr = 0;
        }
    }

//////////////////////////////////
void Server::request( ActionMsg& msg )
    {
    Request::ActionPayload& operation = msg.getPayload();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request: oper=%s", actionToString(operation.m_action)) );
    switch( operation.m_action )
        {
        case eOPENDB:
            openDB(operation);
            break;

        case eCLOSEDB:
            closeDB(operation);
            break;

        case eREAD:
            read(operation);
            break;

        case eWRITE:
            write(operation);
            break;

        case eCLEARDB:
            clearDB(operation);
            break;
        }

    msg.returnToSender();
    }

//////////////////////////////////
void Server::openDB( Request::ActionPayload& operation )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::openDB") );

    // Nothing to do if already opened
    if ( _fdPtr )
        {
        return;
        }

    // Check if the file exists
    bool             newfile = false;
    Jcl::StdFSEntry  db(_dbFname);   
    if ( !db.exists() ) 
        {
        newfile = true;
        }
    
    // More checks on the fname
    if ( !newfile && (db.isDirectory() || !db.isWriteable() || !db.isReadable()) )
        {
        operation._result = _ERR_WRONG_FILE;
        return;
        }

    // Open the DB file (is created if does not already exists)
    _fdPtr = new(_memFd._byteMem) Jcl::StdInputOutputFile(db);

    // Increment the generation number every time the DB is opened
    if ( ++_generation == 0 )
        {
        _generation = 1;   // The Chunk server's generation number is required to never be zero
        }

    // Create signature if new file
    if ( newfile )
        {
        if ( !writeSignature(operation) )
            {
            // Close the file 
            _fdPtr->~StdInputOutputFile();
            _fdPtr = 0;
            return;
            }

        // Report empty file
        operation._result = _EOF;
        }

    // Verify Signature
    else if ( !checkSignature(operation) )
        {
        // Close the file
        _fdPtr->~StdInputOutputFile();
        _fdPtr = 0;
        }
    }

void Server::closeDB( Request::ActionPayload& operation )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::closeDB") );

    // Nothing to do if already closed
    if ( !_fdPtr )
        {
        return;
        }

    // Close the file
    _fdPtr->~StdInputOutputFile();
    _fdPtr = 0;
    }

void Server::clearDB( Request::ActionPayload& operation )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::clearDB") );

    bool dbOpened = false;

    // Close the file if it is already opened
    if ( _fdPtr )
        {
        dbOpened = true;

        // Close the file
        _fdPtr->~StdInputOutputFile();
        _fdPtr = 0;
        }

    // Delete the file
    Jcl::StdFSEntry  db(_dbFname);   
    if ( !db.remove() )
        {
        operation._result = _ERR_FILEIO;
        return;
        }

    // Re-open the file if it was previously opened
    if ( dbOpened )
        {
        openDB( operation );
        }
    }

void Server::read( Request::ActionPayload& operation )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::read") );

    // Trap improper usage
    if ( !_fdPtr )
        {
        Jcl::FatalError::logAndExit( "Rte::Db::Chunk::Server::read - db file NOT opened." );
        }

    // Remember the start of the chunk in the file
    operation._cHandle->_offset = _fdPtr->currentPos();

    // Get the chunk length
    int bytesRead = 0;
    if ( !_fdPtr->read(&(operation._cHandle->_len), Eden::DB::_CLEN_SIZE, bytesRead) || (uint32_t)bytesRead != Eden::DB::_CLEN_SIZE )
        {
        operation._result = _EOF;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::read - _EOF - failed to read Chunk Length (bytesRead=%d)", bytesRead) );
        return;
        }
    if ( operation._cHandle->_len > operation._bufferMaxSize - Eden::DB::_CCRC_SIZE )
        {
        operation._result = _ERR_WRONG_FILE;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::read - _ERR_FILEIO -  Chunk Length greater than buffer (clen=%d, bufSize=%d", operation._cHandle->_len, operation._bufferMaxSize - Eden::DB::_CCRC_SIZE) );
        return;
        }
            
    // Get the chunk data and the CRC
    if ( !_fdPtr->read(operation._buffer, operation._cHandle->_len+Eden::DB::_CCRC_SIZE, bytesRead) || (uint32_t)bytesRead != operation._cHandle->_len+Eden::DB::_CCRC_SIZE )
        {
        operation._result = _ERR_FILEIO;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::read - _ERR_FILEIO -  Failed on read of data+crc (bytesRead=%d)", bytesRead) );
        return;
        }
    
    // Check the CRC of the chunk
    _crcApi.reset();
    _crcApi.accumulate( &(operation._cHandle->_len), Eden::DB::_CLEN_SIZE );
    _crcApi.accumulate( operation._buffer, operation._cHandle->_len+Eden::DB::_CCRC_SIZE );
    if ( !_crcApi.isOkay() )
        {
        operation._result = _CORRUPT_DATA;
        return;
        }

    // If I get here - the chunk was succesfully read.
    // Mark the chunk handle as "associated" with a chunk in the db file
    operation._cHandle->_generation = _generation; 

    // Trap if this is the last chunk in the file
    if ( _fdPtr->isEof() )
        {
        operation._result = _EOF;
        }
    }

void Server::write( Request::ActionPayload& operation )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::write") );

    // Trap improper usage
    if ( !_fdPtr )
        {
        Jcl::FatalError::logAndExit( "Rte::Db::Chunk::Server::write - db file NOT opened." );
        }

    // Check if handle is of the correct DB generation
    if ( operation._cHandle->_generation == _generation )
        {
        _fdPtr->setAbsolutePos( operation._cHandle->_offset );
        writeChunk( operation );
        }

    // Handle is stale
    else 
        {
        // Append chunk to the end of the file
        _fdPtr->setToEOF();
        operation._cHandle->_offset = _fdPtr->currentPos();
        operation._cHandle->_len    = operation._bufferLen;
        if ( writeChunk(operation) )
            {
            // Succesful in adding the chunk to the db file -->mark the handle as "assocaited"
            operation._cHandle->_generation = _generation;
            }
        }
    }


//////////////////////////////////
bool Server::writeChunk( Request::ActionPayload& operation )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::writeChunk") );

    // Write Chunk Length
    int bytesWritten = 0;
    if ( !_fdPtr->write(&(operation._cHandle->_len), Eden::DB::_CLEN_SIZE, bytesWritten) || bytesWritten != Eden::DB::_CLEN_SIZE )
        {
        operation._result = _ERR_FILEIO;
        return false;
        }

    // Write Chunk Data
    if ( !_fdPtr->write(operation._buffer, operation._bufferLen, bytesWritten) || (uint32_t)bytesWritten != operation._bufferLen )
        {
        operation._result = _ERR_FILEIO;
        return false;
        }

    // Calc CRC
    uint32_t crc;
    _crcApi.reset();
    _crcApi.accumulate( &(operation._cHandle->_len), Eden::DB::_CLEN_SIZE );
    _crcApi.accumulate( operation._buffer,operation._bufferLen );
    _crcApi.finalize(&crc);

    // Writ the CRC
    if ( !_fdPtr->write(&crc, Eden::DB::_CCRC_SIZE, bytesWritten) || (uint32_t)bytesWritten != Eden::DB::_CCRC_SIZE )
        {
        operation._result = _ERR_FILEIO;
        return false;
        }

    return true;
    }

bool Server::writeSignature( Request::ActionPayload& operation )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::writeSignature") );

    // Calculate chunk length for the signature
    uint16_t nlen = strlen(_dbSignature);
    uint32_t clen = Eden::DB::_NLEN_SIZE + nlen;

    // Write Chunk Length
    int bytesWritten = 0;
    _fdPtr->setAbsolutePos( 0 );
    if ( !_fdPtr->write(&clen, Eden::DB::_CLEN_SIZE, bytesWritten) || bytesWritten != Eden::DB::_CLEN_SIZE )
        {
        operation._result = _ERR_FILEIO;
        return false;
        }

    // Write Chunk Data
    if ( !_fdPtr->write(&nlen, Eden::DB::_NLEN_SIZE, bytesWritten) || bytesWritten != Eden::DB::_NLEN_SIZE )
        {
        operation._result = _ERR_FILEIO;
        return false;
        }
    if ( !_fdPtr->write(_dbSignature, nlen, bytesWritten) || bytesWritten != nlen )
        {
        operation._result = _ERR_FILEIO;
        return false;
        }

    // Calc CRC
    uint32_t crc;
    _crcApi.reset();
    _crcApi.accumulate( &clen, Eden::DB::_CLEN_SIZE );
    _crcApi.accumulate( &nlen, Eden::DB::_NLEN_SIZE );
    _crcApi.accumulate( (void*)_dbSignature, nlen );
    _crcApi.finalize(&crc);

    // Writ the CRC
    if ( !_fdPtr->write(&crc, Eden::DB::_CCRC_SIZE, bytesWritten) || bytesWritten != Eden::DB::_CCRC_SIZE )
        {
        operation._result = _ERR_FILEIO;
        return false;
        }

    return true;
    }

bool Server::checkSignature( Request::ActionPayload& operation )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::checkSignature") );

    // Get the chunk length
    uint32_t clen      = 0;
    int      bytesRead = 0;
    _fdPtr->setAbsolutePos( 0 );
    if ( !_fdPtr->read(&clen, Eden::DB::_CLEN_SIZE, bytesRead) || bytesRead != Eden::DB::_CLEN_SIZE )
        {
        operation._result = _ERR_FILEIO;
        return false;
        }
    if ( clen > operation._bufferMaxSize - Eden::DB::_CCRC_SIZE )
        {
        operation._result = _ERR_WRONG_FILE;
        return false;
        }
            
    // Get the chunk data and the CRC
    if ( !_fdPtr->read(operation._buffer, clen+Eden::DB::_CCRC_SIZE, bytesRead) || (uint32_t)bytesRead != clen+Eden::DB::_CCRC_SIZE )
        {
        operation._result = _ERR_FILEIO;
        return false;
        }
    
    // Check the CRC of the chunk
    _crcApi.reset();
    _crcApi.accumulate( &clen, Eden::DB::_CLEN_SIZE );
    _crcApi.accumulate( operation._buffer, clen+Eden::DB::_CCRC_SIZE );
    if ( !_crcApi.isOkay() )
        {
        operation._result = _ERR_WRONG_FILE;
        return false;
        }

    // Validate length fields before comparing the signature
    uint16_t nlen = 0;
    memcpy( &nlen, operation._buffer, Eden::DB::_NLEN_SIZE );
    if ( clen != (uint32_t)(nlen + Eden::DB::_NLEN_SIZE) || nlen != strlen(_dbSignature) )
        {
        operation._result = _ERR_WRONG_FILE;
        return false;
        }

    // Check the signature
    if ( strncmp((const char*)(operation._buffer+Eden::DB::_NLEN_SIZE),_dbSignature,nlen) != 0 )
        {
        operation._result = _ERR_WRONG_FILE;
        return false;
        }

    // Trap if this is the last chunk in the file
    if ( _fdPtr->isEof() )
        {
        operation._result = _EOF;
        }

    return true;
    }