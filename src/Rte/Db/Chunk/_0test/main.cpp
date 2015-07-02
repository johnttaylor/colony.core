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

#include "colony_config.h"
#include "Catch/catch.hpp"
#include "Cpl/Checksum/Crc32EthernetFast.h"
#include "Cpl/Itc/MailboxServer.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/File/Api.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Rte/Db/Chunk/Server.h"
#include "Rte/Db/Chunk/FileSystem.h"


/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_chunk(void) {}


#define SECT_     "_0test"


using namespace Rte::Db::Chunk;


///////////////////
#define MAX_BUF_SIZE_       100
#define DB_FNAME_           "myDbFile"
#define SIGNATURE_          "12346789"
#define SIGNATURE_SIZE_     (4+ 2+8 +4) // clen + nlen + strlen(SIGNATURE_) + ccrc


#define LEN_REC1_           10
#define LEN_REC2_           20
#define OFFSET_REC1_        SIGNATURE_SIZE_
#define OFFSET_REC2_        (SIGNATURE_SIZE_ + ( 4 + LEN_REC1_ + 4 ))
 

static uint8_t record1_[LEN_REC1_] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A };
static uint8_t record2_[LEN_REC2_] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA };
static uint8_t buffer_[MAX_BUF_SIZE_];


static Cpl::Checksum::Crc32EthernetFast  chunkCrc_;
static FileSystem                        dbMedia( DB_FNAME_ );
static Server                            chunkServer_( dbMedia, chunkCrc_, "12346789" );
static Handle                            chunkHandle1_;
static Handle                            chunkHandle2_;

static Cpl::Itc::MailboxServer           chunkMailbox_;



////////////////////////////////////////////////////////////////////////////////
void dumpPayload( Rte::Db::Chunk::Request::ActionPayload& payload, const char* msg1, const char* msg2="" )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, (""));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("# PAYLOAD: %s. %s", msg1, msg2 ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("#  action        = %s", Rte::Db::Chunk::Request::actionToString(payload.m_action) ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("#  result        = %s", Rte::Db::Chunk::Request::resultToString(payload.m_result) ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("#  bufLen        = %u", payload.m_bufferLen ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("#  bufferMaxSize = %u", payload.m_bufferMaxSize ));
    if ( payload.m_handlePtr == 0 )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("#  handle        = <none>" ));
        }
    else
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("#  handle        = %p", payload.m_handlePtr ));
        CPL_SYSTEM_TRACE_MSG( SECT_, ("#  handle.offset = %lu", payload.m_handlePtr->m_offset ));
        CPL_SYSTEM_TRACE_MSG( SECT_, ("#  handle.len    = %lu", payload.m_handlePtr->m_len ));
        CPL_SYSTEM_TRACE_MSG( SECT_, ("#  handle.gen    = %lu", payload.m_handlePtr->m_generation ));
        CPL_SYSTEM_TRACE_MSG( SECT_, ("#  buffer[]      = " ));
        unsigned  i;
        for(i=0; i<payload.m_handlePtr->m_len; i++)
            {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("%02X ", payload.m_buffer[i] ));
            }
        CPL_SYSTEM_TRACE_MSG( SECT_, (""));
        }
    }



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "chunk", "[chunk]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Create Thread for the Chunk server to run in
    Cpl::System::Thread* chunkThreadPtr  = Cpl::System::Thread::create( chunkMailbox_,  "CHUNK-LAYER" );


    // Delete the previous DB file (if it exists)
    Cpl::Io::File::Api::remove( DB_FNAME_ );


    // Clear the database
    {
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eCLEARDB, buffer_, MAX_BUF_SIZE_ );
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"CLEAR DB");
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eERR_FILEIO );
    }

    // Open the database
    {
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eOPENDB, buffer_, MAX_BUF_SIZE_ );
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"OPEN DB");
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eEOF );
    }


    // Read 1st Record
    {
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eREAD, buffer_, MAX_BUF_SIZE_, chunkHandle1_ );
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"READ (1st)");
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eEOF );
    REQUIRE( payload.m_handlePtr->m_generation == 0 );
    }

    // Read 2nd Record
    {
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eREAD, buffer_, MAX_BUF_SIZE_, chunkHandle2_ );
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"READ (2nd)");
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eEOF );
    REQUIRE( payload.m_handlePtr->m_generation == 0 );
    }

    // Write Record 1
    {
    memcpy( buffer_, record1_, sizeof(record1_) );
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eWRITE, chunkHandle1_, buffer_, sizeof(record1_) );
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"WRITE", "REC1" );
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eSUCCESS );
    REQUIRE( payload.m_handlePtr->m_generation == 1 );
    }

    // Write Record 2
    {
    memcpy( buffer_, record2_, sizeof(record2_) );
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eWRITE, chunkHandle2_, buffer_, sizeof(record2_) );
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"WRITE", "REC2" );
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eSUCCESS );
    REQUIRE( payload.m_handlePtr->m_generation == 1 );
    }

    // Close the database
    {
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eCLOSEDB,buffer_,MAX_BUF_SIZE_);
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"CLOSE DB");
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eSUCCESS );
    }


    // Open the database
    {
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eOPENDB, buffer_, MAX_BUF_SIZE_ );
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"OPEN DB");
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eSUCCESS );
    }


    // Read 1st Record
    {
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eREAD, buffer_, MAX_BUF_SIZE_, chunkHandle1_ );
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"READ (1st)");
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eSUCCESS );
    REQUIRE( payload.m_handlePtr->m_generation == 2 );
    REQUIRE( payload.m_handlePtr->m_offset == OFFSET_REC1_ );
    REQUIRE( payload.m_handlePtr->m_len    == LEN_REC1_ );
    REQUIRE( payload.m_buffer[0] == 0x01 );
    REQUIRE( payload.m_buffer[9] == 0x0A );
    }

    // Read 2nd Record
    {
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eREAD, buffer_, MAX_BUF_SIZE_, chunkHandle2_ );
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"READ (2nd)");
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eSUCCESS );
    REQUIRE( payload.m_handlePtr->m_generation == 2 );
    REQUIRE( payload.m_handlePtr->m_offset == OFFSET_REC2_ );
    REQUIRE( payload.m_handlePtr->m_len    == LEN_REC2_ );
    REQUIRE( payload.m_buffer[0]  == 0x11 );
    REQUIRE( payload.m_buffer[9]  == 0x1A );
    REQUIRE( payload.m_buffer[19] == 0xFA );
    }

    // Close the database
    {
    Rte::Db::Chunk::Request::ActionPayload payload( Rte::Db::Chunk::Request::eCLOSEDB,buffer_,MAX_BUF_SIZE_);
    Cpl::Itc::SyncReturnHandler            srh;
	Rte::Db::Chunk::Request::ActionMsg     msg(chunkServer_, payload, srh);
    chunkMailbox_.postSync( msg );
    dumpPayload(payload,"CLOSE DB");
    REQUIRE( payload.m_result == Rte::Db::Chunk::Request::eSUCCESS );
    }

    // Shutdown threads
    chunkMailbox_.pleaseStop();
    Cpl::System::Api::sleep(60); // allow time for threads to stop
    REQUIRE( chunkThreadPtr->isRunning() == false );
    Cpl::System::Thread::destroy( *chunkThreadPtr );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0 );





#if 0
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Reading Lines..." ));
    Input fd( "testinput.txt" );
    Cpl::Io::LineReader reader(fd);
    REQUIRE( fd.isOpened() );

    Cpl::Text::FString<6> line;
    REQUIRE( reader.available() == true );
    reader.readln( line );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    REQUIRE( line == "line 1" );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    REQUIRE( line == "line 2" );
    REQUIRE( reader.readln( line ) );
    REQUIRE( fd.length() == TESTINPUT_TXT_FILE_LENGTH );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    line.removeTrailingSpaces();
    REQUIRE( line.isEmpty() );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    REQUIRE( line == "line 4" );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    REQUIRE( line == "line 5" );
    reader.close();
    REQUIRE( fd.isOpened() == false );
    REQUIRE( reader.readln( line ) == false );
    REQUIRE( fd.read( line ) == false );

    //     
    Input fd2( "testinput.txt" );
    REQUIRE( fd2.isOpened() );
    char dummyChar = 29;
    REQUIRE( fd2.setAbsolutePos( TESTINPUT_TEXT_HELLO_OFFEST ) );
    REQUIRE( fd2.length() == TESTINPUT_TXT_FILE_LENGTH );
    REQUIRE( fd2.read(dummyChar) == true );
    REQUIRE( dummyChar == 'A' );

    Cpl::Text::FString<10> buffer("bob");
    REQUIRE( fd2.read(buffer) == true );
    REQUIRE( buffer == "Hello Worl" );

    char myBuffer[10] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
    int  bytesRead    = 1;
    REQUIRE( fd2.available() == true );
    fd2.read( myBuffer, sizeof(myBuffer), bytesRead );
    REQUIRE( bytesRead == 2 );
    REQUIRE( myBuffer[0] == 'd' );
    REQUIRE( myBuffer[1] == '.' );
    REQUIRE( fd2.read( myBuffer, sizeof(myBuffer), bytesRead ) == false );

    fd2.close(); 
    REQUIRE( fd2.isOpened() == false );
    REQUIRE( fd2.read(dummyChar) == false );
#endif
    }
