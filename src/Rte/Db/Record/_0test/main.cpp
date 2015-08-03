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
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/File/Api.h"
#include "Cpl/Io/File/Output.h"
#include "Cpl/Container/SList.h"
#include "Cpl/Container/Map.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Rte/Db/Chunk/Server.h"
#include "Rte/Db/Chunk/FileSystem.h"
#include "Rte/Db/Chunk/Null.h"
#include "Rte/Db/Record/ApiLocal.h"
#include "Rte/Db/Record/Server.h"
#include "Records.h"


/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_record(void) {}

using namespace Rte::Db::Record;


#define SECT_     "_0test"




class MyNullMedia: public Rte::Db::Chunk::Null
{
public:
    /// Constructor
    MyNullMedia(){}


public:
    /// See Rte::Db::Chunk::Media
    Cpl::Io::File::InputOutputApi* openDatabase( bool& newfile ) throw()
        {
        return 0;
        }
};


////////////////////////////////////////////////////////////////////////////////
#define DB_FNAME_               "myDbFile"
#define SIGNATURE_              "12346789"

#define RECORD_BUF_SIZE_        1024

// Create Chunk layer
static Cpl::Checksum::Crc32EthernetFast  chunkCrc_;
static Rte::Db::Chunk::FileSystem        dbMedia( DB_FNAME_ );
static Rte::Db::Chunk::FileSystem        db2Media( DB_FNAME_ );  // Use same file -->excersize incompatible use case
static MyNullMedia                       dbNullMedia;
static Rte::Db::Chunk::Server            chunkServer_(  dbMedia,     chunkCrc_, "12346789" );
static Rte::Db::Chunk::Server            chunk2Server_( db2Media,    chunkCrc_, "123456789" );
static Rte::Db::Chunk::Server            chunk3Server_( dbNullMedia, chunkCrc_, "12346789" );

static Cpl::Itc::MailboxServer           chunkMailbox_;
static Cpl::Itc::MailboxServer           recordLayerMbox_;

static Rte::Db::Chunk::Request::SAP      chunk1SAP_(chunkServer_, chunkMailbox_ );
 
static uint8_t                           buffer_[RECORD_BUF_SIZE_];
static uint8_t                           buffer2_[RECORD_BUF_SIZE_];
static uint8_t                           buffer3_[RECORD_BUF_SIZE_];

static Cpl::Container::Map<ApiLocal>     recordList_( "ignore_this_arg" ); // Use 'static' constructor

// Create Records (which also creates their associate Model Point)
static RecordBar1 modelBar1_( recordList_, recordLayerMbox_, recordLayerMbox_ );
static RecordBar2 modelBar2_( recordList_, recordLayerMbox_, recordLayerMbox_ );

            
// Create Record Server/Layer
static Rte::Db::Record::Server recordLayer_( buffer_, sizeof(buffer_), chunk1SAP_, recordLayerMbox_, recordList_ );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "record", "[record]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Create Threads
    Cpl::System::Thread* chunkThreadPtr       = Cpl::System::Thread::create( chunkMailbox_,     "CHUNK-LAYER" );
    Cpl::System::Thread* upperLayersThreadPtr = Cpl::System::Thread::create( recordLayerMbox_,  "RECORD-LAYER" );


    // Delete the previous DB file (if it exists)
    Cpl::Io::File::Api::remove( DB_FNAME_ );

    
    // TEST: Empty database
    recordLayer_.open();
    Cpl::System::Api::sleep( 300 );
    recordLayer_.close();
    Cpl::System::Api::sleep( 100 );
    
#if 0
    REQUIRE( set1Buffer_[0] == SET1_DEFAULT_VALUE_ );
    REQUIRE( set1Buffer_[1] == SET1_DEFAULT_VALUE_ );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-2] == SET1_DEFAULT_VALUE_ );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-1] == SET1_DEFAULT_VALUE_ );
    REQUIRE( set2Buffer_[0] == SET2_DEFAULT_VALUE_ );
    REQUIRE( set2Buffer_[1] == SET2_DEFAULT_VALUE_ );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-2] == SET2_DEFAULT_VALUE_ );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-1] == SET2_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[0] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[1] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-2] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-1] == SET3_DEFAULT_VALUE_ );

    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client.m_countOpenComplete == 1 );
    REQUIRE( client.m_countOpenFailed == 0 );
    REQUIRE( client.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client.clearCounters();

    // TEST: Populated database
    client.open();
    Cpl::System::Api::sleep( 300 );
    client.close();
    Cpl::System::Api::sleep( 100 );
    
    REQUIRE( set1Buffer_[0] == SET1_DEFAULT_VALUE_ );
    REQUIRE( set1Buffer_[1] == SET1_DEFAULT_VALUE_ );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-2] == SET1_DEFAULT_VALUE_ );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-1] == SET1_DEFAULT_VALUE_ );
    REQUIRE( set2Buffer_[0] == SET2_DEFAULT_VALUE_ );
    REQUIRE( set2Buffer_[1] == SET2_DEFAULT_VALUE_ );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-2] == SET2_DEFAULT_VALUE_ );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-1] == SET2_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[0] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[1] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-2] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-1] == SET3_DEFAULT_VALUE_ );

    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client.m_countOpenComplete == 1 );
    REQUIRE( client.m_countOpenFailed == 0 );
    REQUIRE( client.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client.clearCounters();


    // Create the Record + Set layers for SECOND Database file
    Rte::Db::Chunk::Request::SAP chunk2SAP(chunk2Server_, chunkMailbox_ );
    SetHandler                   client2( buffer2_, sizeof(buffer2_), chunk2SAP, recordLayerMbox_, &errorReporter_ );    // Contains both the Record and Set Handlers
    MySet                        set5( SET5_NAME_, set5Buffer_, sizeof(set5Buffer_), SET5_DEFAULT_VALUE_, client2.m_recordHandler );

    // Register my sets with the Set Handler
    MySetItem set5Item(set5);
    client2.registerSet( set5Item );

    // TEST: Incompatible database file
    client2.open();
    Cpl::System::Api::sleep( 300 );
    client2.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 1 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client2.m_countOpenComplete == 0 );
    REQUIRE( client2.m_countOpenFailed == 1 );
    REQUIRE( client2.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client2.clearCounters();


    // Create the Record + Set layers for THRID Database file
    Rte::Db::Chunk::Request::SAP chunk3SAP(chunk3Server_, chunkMailbox_ );
    SetHandler                   client3( buffer3_, sizeof(buffer3_), chunk3SAP, recordLayerMbox_, &errorReporter_ );    // Contains both the Record and Set Handlers
    MySet                        set6( SET6_NAME_, set6Buffer_, sizeof(set6Buffer_), SET6_DEFAULT_VALUE_, client3.m_recordHandler );

    // Register my sets with the Set Handler
    MySetItem set6Item(set6);
    client3.registerSet( set6Item );

    // TEST: File error on open
    client3.open();
    Cpl::System::Api::sleep( 300 );
    client3.close();
    Cpl::System::Api::sleep( 100 );

    REQUIRE( set6Buffer_[0] == SET6_DEFAULT_VALUE_ );
    REQUIRE( set6Buffer_[1] == SET6_DEFAULT_VALUE_ );
    REQUIRE( set6Buffer_[SET_BUF_SIZE_-2] == SET6_DEFAULT_VALUE_ );
    REQUIRE( set6Buffer_[SET_BUF_SIZE_-1] == SET6_DEFAULT_VALUE_ );

    REQUIRE( errorReporter_.m_errCountFileOpen == 1 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client3.m_countOpenComplete == 0 );
    REQUIRE( client3.m_countOpenFailed == 1 );
    REQUIRE( client3.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client3.clearCounters();



    // TEST: Changed values
    client.open();
    Cpl::System::Api::sleep( 200 );    // Allow time for the DB to be read in BEFORE writting new values
	client.testWrite( SET1_NAME_, 1 );
	client.testWrite( SET2_NAME_, 2 );
	client.testWrite( SET3_NAME_, 3 );
    Cpl::System::Api::sleep( 300 );
    client.close();
    Cpl::System::Api::sleep( 100 );
    
    REQUIRE( set1Buffer_[0] == 1 );
    REQUIRE( set1Buffer_[1] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-2] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-1] == 1 );
    REQUIRE( set2Buffer_[0] == 2 );
    REQUIRE( set2Buffer_[1] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-2] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-1] == 2 );
    REQUIRE( set3Buffer_[0] == 3 );
    REQUIRE( set3Buffer_[1] == 3 );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-2] == 3 );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-1] == 3 );

    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client.m_countOpenComplete == 1 );
    REQUIRE( client.m_countOpenFailed == 0 );
    REQUIRE( client.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client.clearCounters();

    CPL_SYSTEM_TRACE_MSG( SECT_, ( "BEFORE open" ));
    client.open();
    Cpl::System::Api::sleep( 300 );
    client.close();
    Cpl::System::Api::sleep( 100 );
    
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "AFTER close" ));
    REQUIRE( set1Buffer_[0] == 1 );
    REQUIRE( set1Buffer_[1] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-2] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-1] == 1 );
    REQUIRE( set2Buffer_[0] == 2 );
    REQUIRE( set2Buffer_[1] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-2] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-1] == 2 );
    REQUIRE( set3Buffer_[0] == 3 );
    REQUIRE( set3Buffer_[1] == 3 );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-2] == 3 );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-1] == 3 );

    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client.m_countOpenComplete == 1 );
    REQUIRE( client.m_countOpenFailed == 0 );
    REQUIRE( client.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client.clearCounters();

    // TEST: minor upgrade -->add new record
    client.registerSet( set4Item );
    client.open();
    Cpl::System::Api::sleep( 300 );
    client.close();
    Cpl::System::Api::sleep( 100 );
    
    REQUIRE( set1Buffer_[0] == 1 );
    REQUIRE( set1Buffer_[1] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-2] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-1] == 1 );
    REQUIRE( set2Buffer_[0] == 2 );
    REQUIRE( set2Buffer_[1] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-2] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-1] == 2 );
    REQUIRE( set3Buffer_[0] == 3 );
    REQUIRE( set3Buffer_[1] == 3 );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-2] == 3 );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-1] == 3 );
    REQUIRE( set4Buffer_[0] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[1] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[SET_BUF_SIZE_-2] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[SET_BUF_SIZE_-1] == SET4_DEFAULT_VALUE_ );

    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client.m_countOpenComplete == 0 );
    REQUIRE( client.m_countOpenFailed == 1 );
    REQUIRE( client.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client.clearCounters();

    client.open();
    Cpl::System::Api::sleep( 300 );
    client.close();
    Cpl::System::Api::sleep( 100 );
    
    REQUIRE( set1Buffer_[0] == 1 );
    REQUIRE( set1Buffer_[1] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-2] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-1] == 1 );
    REQUIRE( set2Buffer_[0] == 2 );
    REQUIRE( set2Buffer_[1] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-2] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-1] == 2 );
    REQUIRE( set3Buffer_[0] == 3 );
    REQUIRE( set3Buffer_[1] == 3 );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-2] == 3 );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-1] == 3 );
    REQUIRE( set4Buffer_[0] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[1] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[SET_BUF_SIZE_-2] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[SET_BUF_SIZE_-1] == SET4_DEFAULT_VALUE_ );

    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client.m_countOpenComplete == 1 );
    REQUIRE( client.m_countOpenFailed == 0 );
    REQUIRE( client.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client.clearCounters();


    // TEST: Corrupt a Record
    Cpl::Io::File::Output dbfile( DB_FNAME_, false );
    REQUIRE( dbfile.isOpened() );
    REQUIRE( dbfile.setAbsolutePos( 0x50 ) );  // NOTE: Should be corrupting the CHERRY record
    REQUIRE( dbfile.write( '*' ) );
    dbfile.close();
     
    client.open();
    Cpl::System::Api::sleep( 300 );
    client.close();
    Cpl::System::Api::sleep( 100 );
    
    REQUIRE( set1Buffer_[0] == 1 );
    REQUIRE( set1Buffer_[1] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-2] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-1] == 1 );
    REQUIRE( set2Buffer_[0] == 2 );
    REQUIRE( set2Buffer_[1] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-2] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-1] == 2 );
    REQUIRE( set3Buffer_[0] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[1] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-2] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-1] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[0] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[1] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[SET_BUF_SIZE_-2] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[SET_BUF_SIZE_-1] == SET4_DEFAULT_VALUE_ );

    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 1 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client.m_countOpenComplete == 0 );
    REQUIRE( client.m_countOpenFailed == 1 );
    REQUIRE( client.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client.clearCounters();

    client.open();
    Cpl::System::Api::sleep( 300 );
    client.close();
    Cpl::System::Api::sleep( 100 );
    
    REQUIRE( set1Buffer_[0] == 1 );
    REQUIRE( set1Buffer_[1] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-2] == 1 );
    REQUIRE( set1Buffer_[SET_BUF_SIZE_-1] == 1 );
    REQUIRE( set2Buffer_[0] == 2 );
    REQUIRE( set2Buffer_[1] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-2] == 2 );
    REQUIRE( set2Buffer_[SET_BUF_SIZE_-1] == 2 );
    REQUIRE( set3Buffer_[0] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[1] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-2] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set3Buffer_[SET_BUF_SIZE_-1] == SET3_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[0] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[1] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[SET_BUF_SIZE_-2] == SET4_DEFAULT_VALUE_ );
    REQUIRE( set4Buffer_[SET_BUF_SIZE_-1] == SET4_DEFAULT_VALUE_ );

    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    REQUIRE( client.m_countOpenComplete == 1 );
    REQUIRE( client.m_countOpenFailed == 0 );
    REQUIRE( client.m_countStopped == 1 );
    REQUIRE( client.m_countWriteError == 0 );
    errorReporter_.clearCounters();
    client.clearCounters();
#endif

    // Shutdown threads
    chunkMailbox_.pleaseStop();
    recordLayerMbox_.pleaseStop();
    Cpl::System::Api::sleep(100);       // allow time for threads to stop
    REQUIRE( chunkThreadPtr->isRunning() == false );
    REQUIRE( upperLayersThreadPtr->isRunning() == false );
    Cpl::System::Thread::destroy( *chunkThreadPtr );
    Cpl::System::Thread::destroy( *upperLayersThreadPtr );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0 );
    }
