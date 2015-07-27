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
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/File/Api.h"
#include "Cpl/Io/File/Output.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Rte/Db/Chunk/Server.h"
#include "Rte/Db/Chunk/FileSystem.h"
#include "Rte/Db/Chunk/Null.h"
#include "Rte/Db/Record/Core.h"
#include "Rte/Db/Set/Server.h"
#include "Sets.h"


/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_set(void) {}

using namespace Rte::Db::Set;


#define SECT_                   "_0test"

#define RECORD_BUF_SIZE_        1000


////////////////////////////////////////////////////////////////////////////////
class Maker_
{
public:
    ///
    Cpl::Container::Map<Rte::Db::Set::ApiLocal> m_setList;
    Rte::Db::Record::Core                       m_recordHandler;
    Server                                      m_setHandler;
    uint8_t                                     m_buffer[RECORD_BUF_SIZE_];

public:
    /// Constructor
    Maker_( Rte::Db::Chunk::Request::SAP& chunkSAP,
            Cpl::Itc::PostApi&            recordAndSetLayerMbox,
            Cpl::Log::Api&                eventLogger  = Cpl::Log::Loggers::application(),
            Rte::Db::Record::ErrorClient* errorHandler = 0   // Optional
          )
    :m_recordHandler( m_buffer, sizeof(m_buffer), m_setHandler, chunkSAP, recordAndSetLayerMbox, eventLogger, errorHandler )
    ,m_setHandler( m_recordHandler, recordAndSetLayerMbox, m_setList )
        {
        }
};


class ErrorReporter: public Rte::Db::Record::ErrorClient
{
public:
    unsigned m_errCountFileOpen;
    unsigned m_errCountCorruption;
    unsigned m_errCountFileWrite;
    unsigned m_errCountIncompatible;
    unsigned m_countOpened;
    unsigned m_countClosed;

public:
    ErrorReporter()
        :m_errCountFileOpen(0)
        ,m_errCountCorruption(0)
        ,m_errCountFileWrite(0)
        ,m_errCountIncompatible(0)
        ,m_countOpened(0)
        ,m_countClosed(0)
            {}

public:
    void clearCounters()
        {
        m_errCountFileOpen     = 0;
        m_errCountCorruption   = 0;
        m_errCountFileWrite    = 0;
        m_errCountIncompatible = 0;
        m_countOpened          = 0;
        m_countClosed          = 0;
        }

public:
    void reportDbFileOpenError( Rte::Db::Chunk::Request::Result_T errorCode )
        {
        m_errCountFileOpen++;
        }

    void reportDbCorruptionError( void )
        {
        m_errCountCorruption++;
        }

    void reportDbFileWriteError( const char* recordName )
        {
        m_errCountFileWrite++;
        }

    void reportDbIncompatible( void )
        {
        m_errCountIncompatible++;
        }

    void reportDbOpened( void )
        {
        m_countOpened++;
        }

    void reportDbClosed( void )
        {
        m_countClosed++;
        }
};


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

static Cpl::Checksum::Crc32EthernetFast  chunkCrc_;
static Rte::Db::Chunk::FileSystem        dbMedia( DB_FNAME_ );
static Rte::Db::Chunk::FileSystem        db2Media( DB_FNAME_ );  // Use same file -->excersize incompatible use case
static MyNullMedia                       dbNullMedia;
static Rte::Db::Chunk::Server            chunkServer_(  dbMedia,     chunkCrc_, SIGNATURE_ );
static Rte::Db::Chunk::Server            chunk2Server_( db2Media,    chunkCrc_, "123456789" );
static Rte::Db::Chunk::Server            chunk3Server_( dbNullMedia, chunkCrc_, SIGNATURE_ );

static Cpl::Itc::MailboxServer           chunkMailbox_;
static Cpl::Itc::MailboxServer           upperLayersMailbox_;
static Cpl::Itc::MailboxServer           modelMailbox_;

static ErrorReporter                     errorReporter_;

static ModelBar1                         modelBar1_( modelMailbox_ );
static ModelBar2                         modelBar2_( modelMailbox_ );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "record", "[record]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Create Threads
    Cpl::System::Thread* chunkThreadPtr       = Cpl::System::Thread::create( chunkMailbox_,        "CHUNK-LAYER" );
    Cpl::System::Thread* upperLayersThreadPtr = Cpl::System::Thread::create( upperLayersMailbox_,  "RECORD/SET-LAYERS" );
    Cpl::System::Thread* modelThreadPtr       = Cpl::System::Thread::create( modelMailbox_,  "MODEL" );


    // Delete the previous DB file (if it exists)
    Cpl::Io::File::Api::remove( DB_FNAME_ );

    // Create the Record + Set layers
    Rte::Db::Chunk::Request::SAP chunkSAP(chunkServer_, chunkMailbox_ );
    Maker_* setLayerPtr = new Maker_( chunkSAP, upperLayersMailbox_, Cpl::Log::Loggers::application(), &errorReporter_ ); 
    SetBar1* setBar1Ptr = new SetBar1( setLayerPtr->m_setList, modelBar1_, upperLayersMailbox_, upperLayersMailbox_, setLayerPtr->m_setHandler );
    SetBar2* setBar2Ptr = new SetBar2( setLayerPtr->m_setList, modelBar2_, upperLayersMailbox_, upperLayersMailbox_, setLayerPtr->m_setHandler );
    
    // TEST: Empty database
    setLayerPtr->m_setHandler.open();
    Cpl::System::Api::sleep( 500 );
    setLayerPtr->m_setHandler.close();
    Cpl::System::Api::sleep( 300 );

    REQUIRE( errorReporter_.m_errCountFileOpen == 0 );
    REQUIRE( errorReporter_.m_errCountCorruption == 0 );
    REQUIRE( errorReporter_.m_errCountFileWrite == 0 );
    REQUIRE( errorReporter_.m_errCountIncompatible == 0 );
    REQUIRE( errorReporter_.m_countOpened == 1 );
    REQUIRE( errorReporter_.m_countClosed == 1 );
    errorReporter_.clearCounters();

    // Clean up
    delete setBar2Ptr;
    delete setBar1Ptr;
    delete setLayerPtr;

    // Shutdown threads
    chunkMailbox_.pleaseStop();
    upperLayersMailbox_.pleaseStop();
    Cpl::System::Api::sleep(100);       // allow time for threads to stop
    REQUIRE( chunkThreadPtr->isRunning() == false );
    REQUIRE( upperLayersThreadPtr->isRunning() == false );
    Cpl::System::Thread::destroy( *chunkThreadPtr );
    Cpl::System::Thread::destroy( *upperLayersThreadPtr );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0 );
    }
