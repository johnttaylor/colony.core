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
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Rte/Db/Chunk/Server.h"
#include "Rte/Db/Chunk/FileSystem.h"
#include "Rte/Db/Chunk/Null.h"
#include "Rte/Db/Record/Api.h"
#include "Rte/Db/Record/Handler.h"
#include "Rte/Db/Record/Core.h"


/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_record(void) {}

using namespace Rte::Db::Record;


#define SECT_     "_0test"



////////////////////////////////////////////////////////////////////////////////
class MySet: public Api
{                               
public:
    Rte::Db::Chunk::Handle  m_chunkHdl;
    const char*             m_name;
    uint8_t*                m_dataPtr;
    uint32_t                m_datalen;
    Handler&                m_writer;
    uint8_t                 m_defaultValue;

public:
    ///
    MySet( const char* name, uint8_t* dataPtr,  uint32_t dataLen, uint8_t defaultValue, Handler& writeApi )
        :m_name(name)
        ,m_dataPtr(dataPtr)
        ,m_datalen(dataLen)
        ,m_writer(writeApi)
        ,m_defaultValue(defaultValue)
            {
            }

public:
    ///
    const char* getName(void) const                     { return m_name; }
    ///
    void setChunkHandle( Rte::Db::Chunk::Handle& src )  { m_chunkHdl = src; }
    ///
    Rte::Db::Chunk::Handle& getChunkHandle(void)        { return m_chunkHdl; }

    ///
    bool notifyRead( void* srcBuffer, uint32_t dataLen )
        {
        if ( dataLen > m_datalen )
            {
            CPL_SYSTEM_TRACE_MSG(SECT_, ("MySet::notifyRead. Buffer length Error, setLen=%lu, srcLen=%lu.", m_datalen, dataLen) );
            return false;
            }

        memcpy( m_dataPtr, srcBuffer, dataLen );
        CPL_SYSTEM_TRACE_MSG(SECT_, ("MySet::notifyRead. inLen=%u, [0]=0x%x, [n-1]=0x%x", dataLen, m_dataPtr[0], m_dataPtr[m_datalen-1] ));
        return true;
        }

    ///
    uint32_t fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize )
        {
        CPL_SYSTEM_TRACE_MSG(SECT_, ("MySet::fillWriteBuffer. [0]=0x%x, [n-1]=0x%x", m_dataPtr[0], m_dataPtr[m_datalen-1] ));
        if ( maxDataSize < m_datalen )
            {
            Cpl::System::FatalError::logf( "MySet::fillWriteBuffer - Buffer length Error. setLen=%lu, dstMaxLen=%lu.", m_datalen, maxDataSize );
            }

        memcpy(dstBuffer, m_dataPtr, m_datalen);
        return m_datalen; 
        }

    ///
    void notifyWriteDone(void)
        {
        CPL_SYSTEM_TRACE_MSG(SECT_, ("MySet::notifyWriteDone"));
        }

public:
    ///
    void defaultSet(void) throw()
        {
        memset(m_dataPtr, m_defaultValue, m_datalen );
        }
};

///
class MySetItem: public Cpl::Container::Item
{
public:
    MySet&  m_set;

public:
    MySetItem( MySet& set ): m_set(set){}

public:
    MySet& getSet() { return m_set; }
};


////////////////////////////////////////////////////////////////////////////////
class TestWriteRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<TestWriteRequest> SAP;

public:
    /// Payload for Message: Writing
    class TestWritePayload
    {
    public:
        ///
        const char* m_setname;
        ///
        uint8_t     m_newvalue;

    public:
        ///
        TestWritePayload(const char* setname, uint8_t newvalue):m_setname(setname),m_newvalue(newvalue){};
    };

    /// Message Type: Write
    typedef Cpl::Itc::RequestMessage<TestWriteRequest,TestWritePayload> TestWriteMsg;
    


public:
    /// Request: Write
    virtual void request( TestWriteMsg& msg ) = 0;
    
};




////////////////////////////////////////////////////////////////////////////////
class SetHandler: public Client,
                  public Cpl::Container::SList<MySetItem>,
                  public Cpl::Itc::CloseSync,
                  public TestWriteRequest
{
public:
    int                                 m_count;
    Cpl::Itc::CloseRequest::CloseMsg*   m_pendingCloseMsgPtr;
    Core                                m_recordHandler;
    bool                                m_opened;

public:
    unsigned m_countOpenComplete;
    unsigned m_countOpenFailed;
    unsigned m_countStopped;
    unsigned m_countWriteError;


public:
    ///
    SetHandler( uint8_t* buffer, uint32_t bufsize, Rte::Db::Chunk::Request::SAP& chunkSAP, Cpl::Itc::PostApi& upperLayersMbox, ErrorClient* errHandlerPtr=0 )
        :Cpl::Itc::CloseSync(upperLayersMbox)
        ,m_count(0)
        ,m_pendingCloseMsgPtr(0)
        ,m_recordHandler(buffer, bufsize, *this, chunkSAP, upperLayersMbox, Cpl::Log::Loggers::null(), errHandlerPtr )
        ,m_countOpenComplete(0)
        ,m_countOpenFailed(0)
        ,m_countStopped(0)
        ,m_countWriteError(0)
        ,m_opened(false)
            {
            }


public:
    /// 
    void registerSet( MySetItem& setToRegister )
        {
        put( setToRegister );
        }

    /// 
    void clearCounters()
        {
        m_countOpenComplete  = 0;
        m_countOpenFailed    = 0;
        m_countStopped       = 0;
        m_countWriteError    = 0;
        }

    ///
	void testWrite( const char* setname, uint8_t newValue ) throw()
        {
	    TestWritePayload              payload( setname, newValue );
        Cpl::Itc::SyncReturnHandler   srh;
        TestWriteMsg 	              msg(*this,payload,srh);
        m_mbox.postSync(msg);
        }


public:
    ///
    Api* getRecordApi( const char* recName, uint16_t nameLen )
        {
        Cpl::Text::FString<16> recNameString;
        recNameString.copyIn( recName, nameLen );

        MySetItem* itemPtr = first();
        while( itemPtr )
            {
            MySet& set = itemPtr->getSet();

            if ( strncmp( recName, set.getName(), nameLen ) == 0 )
                {
                CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::getRecordApi, name=%s, found=%p, old count=%d", recNameString(), &set, m_count));
                m_count--;
                return &set;
                }

            itemPtr = next( *itemPtr );
            }

        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::getRecordApi, NO MATCH FOUND FOR: name=%s, old count=%d", recNameString(), m_count));
        return 0;
        }

    ///
    void notifyOpenCompleted(void)
        {
        m_countOpenComplete++;
        issueWrites();
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::notifyOpenCompleted. count=%d", m_count));
        }

    ///
    void notifyOpenedWithErrors(Rte::Db::Record::Client::OpenError_T errorCode) 
        {
        m_countOpenFailed++;
        issueWrites();
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::notifyOpenedWithErrors. Code=%d, count=%d", errorCode, m_count));
        }

    ///
    bool isCleanLoad(void)
        {
        return m_count == 0;
        }

    ///
    void notifyWriteError(void)
        {
        m_countWriteError++;
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::notifyWriteError.count=%d", m_countWriteError));
        }

    ///
    void notifyStopped(void)
        {
        m_countStopped++;
        m_opened = false;
        if ( m_pendingCloseMsgPtr )
            {
            m_pendingCloseMsgPtr->returnToSender();
            m_pendingCloseMsgPtr = 0;
            }
        }


public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
        {
        m_count  = countSets();
        m_opened = true;
        defaultSets();
        m_recordHandler.start();
        msg.returnToSender();
        }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
        {
        // Do nothing if ALREADY in the stopped/idle state
        if ( m_opened )
            {
            m_recordHandler.stop();
            m_pendingCloseMsgPtr = &msg;
            }
        else
            {
            msg.returnToSender();
            }
        }

    void request( TestWriteMsg& msg )
        {
        const char* setName  = msg.getPayload().m_setname;
        uint8_t     newvalue = msg.getPayload().m_newvalue;

        MySetItem* itemPtr = first();
        while( itemPtr )
            {
            MySet& set = itemPtr->getSet();

            if ( strcmp( setName, set.getName()) == 0 )
                {
                memset(set.m_dataPtr, newvalue, set.m_datalen );
                CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::TestWriteMsg, name=%s, newValue=%x, [0]=0x%x, [n-1]=0x%x", setName, newvalue, set.m_dataPtr[0], set.m_dataPtr[set.m_datalen-1] ));
                m_recordHandler.write( set );
                msg.returnToSender();
                return;
                }

            itemPtr = next( *itemPtr );
            }

        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::TestWriteMsg, NO MATCH FOUND FOR: name=%s", setName ));
        msg.returnToSender();
        }

public:
    ///
    void issueWrites(void)
        {
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::issueWrites"));
        MySetItem* itemPtr = first();
        while( itemPtr )
            {
            m_recordHandler.write( itemPtr->getSet() );
            itemPtr = next(*itemPtr);
            }
        }

    ///
    void defaultSets(void)
        {
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::defaultSets"));
        MySetItem* itemPtr = first();
        while( itemPtr )
            {
            itemPtr->getSet().defaultSet();
            itemPtr = next(*itemPtr);
            }
        }

    /// 
    int countSets(void)
        {
        int count = 0;
        MySetItem* itemPtr = first();
        while( itemPtr )
            {
            count++;
            itemPtr = next(*itemPtr);
            }

        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::countSets. count=%d", count));
        return count;
        }
};


class ErrorReporter: public ErrorClient
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

#define RECORD_BUF_SIZE_        100
#define SET_BUF_SIZE_           10

#define SET1_NAME_              "plum"
#define SET2_NAME_              "apple"
#define SET3_NAME_              "cherry"
#define SET4_NAME_              "pineapple"
#define SET5_NAME_              "tomato"
#define SET6_NAME_              "peas"

#define SET1_DEFAULT_VALUE_     0xAA
#define SET2_DEFAULT_VALUE_     0x55
#define SET3_DEFAULT_VALUE_     0x88
#define SET4_DEFAULT_VALUE_     0xCC
#define SET5_DEFAULT_VALUE_     0xEE
#define SET6_DEFAULT_VALUE_     0xDD


static Cpl::Checksum::Crc32EthernetFast  chunkCrc_;
static Rte::Db::Chunk::FileSystem        dbMedia( DB_FNAME_ );
static Rte::Db::Chunk::FileSystem        db2Media( DB_FNAME_ );  // Use same file -->excersize incompatible use case
static MyNullMedia                       dbNullMedia;
static Rte::Db::Chunk::Server            chunkServer_(  dbMedia,     chunkCrc_, "12346789" );
static Rte::Db::Chunk::Server            chunk2Server_( db2Media,    chunkCrc_, "123456789" );
static Rte::Db::Chunk::Server            chunk3Server_( dbNullMedia, chunkCrc_, "12346789" );

static Cpl::Itc::MailboxServer           chunkMailbox_;
static Cpl::Itc::MailboxServer           upperLayersMailbox_;

static uint8_t                           buffer_[RECORD_BUF_SIZE_];
static uint8_t                           buffer2_[RECORD_BUF_SIZE_];
static uint8_t                           buffer3_[RECORD_BUF_SIZE_];

static uint8_t                           set1Buffer_[SET_BUF_SIZE_];
static uint8_t                           set2Buffer_[SET_BUF_SIZE_];
static uint8_t                           set3Buffer_[SET_BUF_SIZE_];
static uint8_t                           set4Buffer_[SET_BUF_SIZE_];
static uint8_t                           set5Buffer_[SET_BUF_SIZE_];
static uint8_t                           set6Buffer_[SET_BUF_SIZE_];

static ErrorReporter                     errorReporter_;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "record", "[record]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Create Threads
    Cpl::System::Thread* chunkThreadPtr       = Cpl::System::Thread::create( chunkMailbox_,        "CHUNK-LAYER" );
    Cpl::System::Thread* upperLayersThreadPtr = Cpl::System::Thread::create( upperLayersMailbox_,  "RECORD/SET-LAYERS" );


    // Delete the previous DB file (if it exists)
    Cpl::Io::File::Api::remove( DB_FNAME_ );

    // Create the Record + Set layers
    Rte::Db::Chunk::Request::SAP chunkSAP(chunkServer_, chunkMailbox_ );
    SetHandler                   client( buffer_, sizeof(buffer_), chunkSAP, upperLayersMailbox_, &errorReporter_ );    // Contains both the Record and Set Handlers
    MySet                        set1( SET1_NAME_, set1Buffer_, sizeof(set1Buffer_), SET1_DEFAULT_VALUE_, client.m_recordHandler );
    MySet                        set2( SET2_NAME_, set2Buffer_, sizeof(set2Buffer_), SET2_DEFAULT_VALUE_, client.m_recordHandler );
    MySet                        set3( SET3_NAME_, set3Buffer_, sizeof(set3Buffer_), SET3_DEFAULT_VALUE_, client.m_recordHandler );
    MySet                        set4( SET4_NAME_, set4Buffer_, sizeof(set4Buffer_), SET4_DEFAULT_VALUE_, client.m_recordHandler );
    

    // Register my sets with the Set Handler
    MySetItem set1Item(set1);
    MySetItem set2Item(set2);
    MySetItem set3Item(set3);
    MySetItem set4Item(set4);
    client.registerSet( set1Item );
    client.registerSet( set2Item );
    client.registerSet( set3Item );

    // TEST: Empty database
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
    SetHandler                   client2( buffer2_, sizeof(buffer2_), chunk2SAP, upperLayersMailbox_, &errorReporter_ );    // Contains both the Record and Set Handlers
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
    SetHandler                   client3( buffer3_, sizeof(buffer3_), chunk3SAP, upperLayersMailbox_, &errorReporter_ );    // Contains both the Record and Set Handlers
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
