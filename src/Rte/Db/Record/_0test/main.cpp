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


///////////////////////////////////
static const char* b2t_( bool flag )
    {
    return flag? "TRUE": "false";
    }

static const char* n2s_( const char* elementValue, bool isValid )
    {
    if ( isValid )
        {
        return elementValue;
        }
    else
        {
        return "?invalid?";
        }
    }

static void display_bar1_( PointBar1 point, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "== %s", msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "   t1.f1 = [%s]", n2s_( point.m_fields1.m_text1.get(),  point.m_fields1.m_text1.isValid()) ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "   t1.f3 = [%s]", n2s_( point.m_fields1.m_text3.get(),  point.m_fields1.m_text3.isValid()) ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "   t1.f8 = [%s]", n2s_( point.m_fields1.m_text8.get(),  point.m_fields1.m_text8.isValid()) ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "   t2.f10= [%s]", n2s_( point.m_fields2.m_text10.get(), point.m_fields2.m_text10.isValid()) ));
    }

static void display_TupleFoo2_( TupleFoo2 tuple, unsigned idx )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "   index: %u", idx ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "     in_container_ = %s", b2t_( tuple.isInContainer() ) ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "     f3 = [%s]", n2s_( tuple.m_text3.get(),  tuple.m_text3.isValid()) ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "     f5 = [%s]", n2s_( tuple.m_text5.get(),  tuple.m_text5.isValid()) ));
    }


static void display_bar2_( PointBar2 point, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "== %s", msg ));
    unsigned i;
    for(i=0; i<point.getNumTuples(); i++)
        {                    
        display_TupleFoo2_( point[i], i );
        }
    }

static void display_bar2ext_( PointBar2Ext point, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "== %s", msg ));
    unsigned i;
    for(i=0; i<point.getNumTuples(); i++)
        {                    
        display_TupleFoo2_( point[i], i );
        }
    }


////////////////////////////////////////////////////////////////////////////////
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



////////////////////////////////////////////
class HealthMonitor: public Cpl::Itc::CloseSync,
                     public HealthResponse
{
public: // Make public to simply testing
    ///
    HealthRequest::RegisterPayload      m_healthRegisterPayload;
    ///
    HealthResponse::RegisterMsg         m_healthRegisterMsg;
    ///
    HealthRequest::CancelPayload        m_healthCancelPayload;
    ///
    HealthResponse::CancelMsg           m_healthCancelMsg;
    ///
    HealthRequest::SAP&                 m_healthSAP;
    ///
    Cpl::Itc::CloseRequest::CloseMsg*   m_closeMsgPtr;
    ///
    HealthRequest::Status_T             m_status;
    ///
    unsigned                            m_openingCount;
    ///
    unsigned                            m_runningCount;
    ///
    unsigned                            m_runningUpgradeCount;
    ///
    unsigned                            m_runningCorruptCount;
    ///
    unsigned                            m_errMediaCount;
    ///
    unsigned                            m_errSchemaCount;
    ///
    unsigned                            m_closingCount;
    ///
    unsigned                            m_closedCount;
    ///
    bool                                m_ownHealthMsg;
                                                                                   

public:
    ///
    HealthMonitor( Cpl::Itc::PostApi& healthMbox, HealthRequest::SAP& recordHandleSAP )
        :CloseSync(healthMbox)
        ,m_healthRegisterPayload( HealthRequest::eUNKNOWN )
        ,m_healthRegisterMsg( *this, healthMbox, recordHandleSAP, m_healthRegisterPayload )
        ,m_healthCancelPayload( m_healthRegisterMsg.getRequestMsg() )
        ,m_healthCancelMsg( *this, healthMbox, recordHandleSAP, m_healthCancelPayload )
        ,m_healthSAP( recordHandleSAP )
        ,m_closeMsgPtr(0)
        ,m_status(HealthRequest::eUNKNOWN)
        ,m_openingCount(0)
        ,m_runningCount(0)
        ,m_runningUpgradeCount(0)
        ,m_runningCorruptCount(0)
        ,m_errMediaCount(0)
        ,m_errSchemaCount(0)
        ,m_closingCount(0)
        ,m_closedCount(0)
        ,m_ownHealthMsg(true)
            {
            }


protected: // Cpl::Itc::Close/Open
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "HealthMonitor::request( OpenMsg ): starting health monitoring..." ));
        m_status              = HealthRequest::eUNKNOWN;
        m_openingCount        = 0;       
        m_runningCount        = 0;  
        m_runningUpgradeCount = 0;   
        m_runningCorruptCount = 0;  
        m_errMediaCount       = 0;       
        m_errSchemaCount      = 0;       
        m_closingCount        = 0;       
        m_closedCount         = 0;     
        m_ownHealthMsg        = false;

        m_healthRegisterPayload.m_status = m_status;
        m_healthSAP.post( m_healthRegisterMsg.getRequestMsg() );  // Register for health notification
        msg.returnToSender();
        }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "HealthMonitor::request( CloseMsg ): stopping health monitoring..." ));
        if ( m_ownHealthMsg )
            {
            msg.returnToSender();
            }
        else
            {
            m_closeMsgPtr = &msg;        
            m_healthSAP.post( m_healthCancelMsg.getRequestMsg() );  // Cancel for health notification
            }
        }

protected:
    /// 
    void response( CancelMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "HealthMonitor::response( CancelMsg ): health monitoring STOPPED." ));
        
        m_ownHealthMsg = true;
        if ( m_closeMsgPtr )
            {
            m_closeMsgPtr->returnToSender();
            m_closeMsgPtr = 0;
            }
        }

    ///
    void response( RegisterMsg& msg )
        {
        const char* statusString = 0;
        m_status = msg.getPayload().m_status;
        switch ( m_status )
            {
            case HealthRequest::eOPENING: 
                statusString = "eOPENING";
                m_openingCount++;
                break;

            case HealthRequest::eRUNNING: 
                statusString = "eRUNNING";
                m_runningCount++;
                break;

            case HealthRequest::eRUNNING_MINOR_UPGRADE: 
                statusString = "eRUNNING_MINOR_UPGRADE";
                m_runningUpgradeCount++;
                break;

            case HealthRequest::eNO_STORAGE_MEDIA_ERR: 
                statusString = "eNO_STORAGE_MEDIA_ERR";
                m_errMediaCount++;
                break;

            case HealthRequest::eRUNNING_CORRUPTED_INPUT: 
                statusString = "eRUNNING_CORRUPTED_INPUT";
                m_runningCorruptCount++;
                break;

            case HealthRequest::eNO_STORAGE_WRONG_SCHEMA: 
                statusString = "eNO_STORAGE_WRONG_SCHEMA";
                m_errSchemaCount++;
                break;

            case HealthRequest::eCLOSING: 
                statusString = "eCLOSING";
                m_closingCount++;
                break;

            case HealthRequest::eCLOSED: 
                statusString = "eCLOSED";
                m_closedCount++;
                break;

            default:
                statusString = "****** Oops - unexpectd value!";
                break;
            }

        
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "HealthMonitor::response( RegisterMsg ): Health Notification = %s", statusString ));
        m_healthSAP.post( m_healthRegisterMsg.getRequestMsg() );  // Register for health notification
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
static Cpl::Itc::MailboxServer           monitorMbox_;

static Rte::Db::Chunk::Request::SAP      chunk1SAP_(chunkServer_, chunkMailbox_ );
static Rte::Db::Chunk::Request::SAP      chunk2SAP_(chunk2Server_, chunkMailbox_ );
static Rte::Db::Chunk::Request::SAP      chunk3SAP_(chunk3Server_, chunkMailbox_ );
 
static uint8_t                           buffer_[RECORD_BUF_SIZE_];
static uint8_t                           buffer2_[RECORD_BUF_SIZE_];
static uint8_t                           buffer3_[RECORD_BUF_SIZE_];

static Cpl::Container::Map<ApiLocal>     recordList_( "ignore_this_arg" ); // Use 'static' constructor
static Cpl::Container::Map<ApiLocal>     recordList2_( "ignore_this_arg" ); // Use 'static' constructor
static Cpl::Container::Map<ApiLocal>     recordList3_( "ignore_this_arg" ); // Use 'static' constructor
static Cpl::Container::Map<ApiLocal>     recordList4_( "ignore_this_arg" ); // Use 'static' constructor

// Create Records (which also creates their associate Model Point)
static RecordBar1 modelBar1_( recordList_, recordLayerMbox_, recordLayerMbox_ );
static RecordBar2 modelBar2_( recordList_, recordLayerMbox_, recordLayerMbox_ );

static RecordBar1 modelBar1a_( recordList2_, recordLayerMbox_, recordLayerMbox_ );
static RecordBar2 modelBar2a_( recordList2_, recordLayerMbox_, recordLayerMbox_ );

static RecordBar1 modelBar1b_( recordList3_, recordLayerMbox_, recordLayerMbox_ );
static RecordBar2 modelBar2b_( recordList3_, recordLayerMbox_, recordLayerMbox_ );
            
static RecordBar1    modelBar1Ext_( recordList4_, recordLayerMbox_, recordLayerMbox_ );
static RecordBar2Ext modelBar2Ext_( recordList4_, recordLayerMbox_, recordLayerMbox_ );
            
// Create Record Server/Layer
static Rte::Db::Record::Server recordLayer_( buffer_, sizeof(buffer_), chunk1SAP_, recordLayerMbox_, recordList_ );
static Rte::Db::Record::Server recordLayer2_( buffer2_, sizeof(buffer2_), chunk2SAP_, recordLayerMbox_, recordList2_ );
static Rte::Db::Record::Server recordLayer3_( buffer3_, sizeof(buffer3_), chunk3SAP_, recordLayerMbox_, recordList3_ );
static Rte::Db::Record::Server recordLayer4_( buffer_, sizeof(buffer_), chunk1SAP_, recordLayerMbox_, recordList4_ );




////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "record", "[record]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Create Monitors
    HealthMonitor  myMonitor( monitorMbox_, recordLayer_.getHealthSAP() );
    HealthMonitor  myMonitor2( monitorMbox_, recordLayer2_.getHealthSAP() );
    HealthMonitor  myMonitor3( monitorMbox_, recordLayer3_.getHealthSAP() );
    HealthMonitor  myMonitor4( monitorMbox_, recordLayer4_.getHealthSAP() );

    // Create Threads
    Cpl::System::Thread* chunkThreadPtr       = Cpl::System::Thread::create( chunkMailbox_,     "CHUNK-LAYER" );
    Cpl::System::Thread* recordLayerThreadPtr = Cpl::System::Thread::create( recordLayerMbox_,  "RECORD-LAYER" );
    Cpl::System::Thread* monitorThreadPtr     = Cpl::System::Thread::create( monitorMbox_,      "monitor" );


    // Delete the previous DB file (if it exists)
    Cpl::Io::File::Api::remove( DB_FNAME_ );

    
    // 
    // TEST: Empty database
    //

    // Default empty DB to application defaults
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "################### Opening empty DB file..." ));
    myMonitor.open();
    REQUIRE( recordLayer_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor.m_status == HealthRequest::eRUNNING );
    recordLayer_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor.m_status == HealthRequest::eCLOSED );
    
    // Re-open the DB and Verify defaults
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Verifying initial defaults..." ));
    REQUIRE( recordLayer_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor.m_status == HealthRequest::eRUNNING );
    QueryBar1 query1(modelBar1_);
    query1.issueQuery();
    display_bar1_( query1, "Query Bar1: Verify initial defaults" );
    REQUIRE( query1.m_fields1.m_text1.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T1 );
    REQUIRE( query1.m_fields1.m_text3.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T3 );
    REQUIRE( query1.m_fields1.m_text8.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T8 );
    REQUIRE( query1.m_fields2.m_text10.isValid() == REC_BAR1_TUP2_DEFAULT_VALID_T10 );
    if ( query1.m_fields1.m_text1.isValid() )  { REQUIRE( query1.m_fields1.m_text1.getString()  == REC_BAR1_TUP1_DEFAULT_T1 ); }
    if ( query1.m_fields1.m_text3.isValid() )  { REQUIRE( query1.m_fields1.m_text3.getString()  == REC_BAR1_TUP1_DEFAULT_T3 ); }
    if ( query1.m_fields1.m_text8.isValid() )  { REQUIRE( query1.m_fields1.m_text8.getString()  == REC_BAR1_TUP1_DEFAULT_T8 ); }
    if ( query1.m_fields2.m_text10.isValid() ) { REQUIRE( query1.m_fields2.m_text10.getString() == REC_BAR1_TUP2_DEFAULT_T10 ); }

    QueryBar2 query2(modelBar2_);
    query2.issueQuery();
    display_bar2_( query2, "Query Bar2: Verify initial defaults" );
    REQUIRE( query2[0].isInContainer() == REC_BAR2_TUP0_DEFAULT_INCONTAINER );
    if ( query2[0].isInContainer() )
        {
        REQUIRE( query2[0].m_text3.isValid() == REC_BAR2_TUP0_DEFAULT_VALID_T3 );
        REQUIRE( query2[0].m_text5.isValid() == REC_BAR2_TUP0_DEFAULT_VALID_T5 );
        if ( query2[0].m_text3.isValid() ) { REQUIRE( query2[0].m_text3.getString() == REC_BAR2_TUP0_DEFAULT_T3 ); }
        if ( query2[0].m_text5.isValid() ) { REQUIRE( query2[0].m_text5.getString() == REC_BAR2_TUP0_DEFAULT_T5 ); }
        }
    REQUIRE( query2[1].isInContainer() == REC_BAR2_TUP1_DEFAULT_INCONTAINER );
    if ( query2[1].isInContainer() )
        {
        REQUIRE( query2[1].m_text3.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T3 );
        REQUIRE( query2[1].m_text5.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T5 );
        if ( query2[1].m_text3.isValid() ) { REQUIRE( query2[1].m_text3.getString() == REC_BAR2_TUP1_DEFAULT_T3 ); }
        if ( query2[1].m_text5.isValid() ) { REQUIRE( query2[1].m_text5.getString() == REC_BAR2_TUP1_DEFAULT_T5 ); }
        }

    REQUIRE( query2[2].isInContainer() == REC_BAR2_TUP2_DEFAULT_INCONTAINER );
    if ( query2[2].isInContainer() )
        {
        REQUIRE( query2[2].m_text3.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T3 );
        REQUIRE( query2[2].m_text5.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T5 );
        if ( query2[2].m_text3.isValid() ) { REQUIRE( query2[2].m_text3.getString() == REC_BAR2_TUP2_DEFAULT_T3 ); }
        if ( query2[2].m_text5.isValid() ) { REQUIRE( query2[2].m_text5.getString() == REC_BAR2_TUP2_DEFAULT_T5 ); }
        }

    REQUIRE( query2[3].isInContainer() == REC_BAR2_TUP3_DEFAULT_INCONTAINER );
    if ( query2[3].isInContainer() )
        {
        REQUIRE( query2[3].m_text3.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T3 );
        REQUIRE( query2[3].m_text5.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T5 );
        if ( query2[3].m_text3.isValid() ) { REQUIRE( query2[3].m_text3.getString() == REC_BAR2_TUP3_DEFAULT_T3 ); }
        if ( query2[3].m_text5.isValid() ) { REQUIRE( query2[3].m_text5.getString() == REC_BAR2_TUP3_DEFAULT_T5 ); }
        }


    recordLayer_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor.m_status == HealthRequest::eCLOSED );
    REQUIRE( myMonitor.m_openingCount == 2 );
    REQUIRE( myMonitor.m_runningCount == 2 );
    REQUIRE( myMonitor.m_runningUpgradeCount == 0 );
    REQUIRE( myMonitor.m_runningCorruptCount == 0 );
    REQUIRE( myMonitor.m_errMediaCount == 0 );
    REQUIRE( myMonitor.m_errSchemaCount == 0 );
    REQUIRE( myMonitor.m_closingCount == 2 );
    REQUIRE( myMonitor.m_closedCount == 3 );
    myMonitor.close();


    //
    // Test: Write to the DB
    //
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "" ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "################### Opening defaulted DB file..." ));
    myMonitor.open();
    REQUIRE( recordLayer_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor.m_status == HealthRequest::eRUNNING );

    ControllerBar1 writerBar1( modelBar1_ );
    writerBar1.setAllInUseState(false);
    writerBar1.m_fields1.m_text1.setInUse();
    writerBar1.m_fields1.m_text1.setValid();
    writerBar1.m_fields1.m_text1.set( "*" );
    writerBar1.m_fields2.m_text10.setInUse();
    writerBar1.m_fields2.m_text10.setValid();
    writerBar1.m_fields2.m_text10.set( "bobs not here" );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Updating ModelBar1...."));
    writerBar1.updateModel();
    ControllerBar2 writerBar2( modelBar2_ );
    int idx = 0;
    writerBar2.m_tuples_[idx].setAllInUseState(true);
    writerBar2.m_tuples_[idx].setAllValidState(RTE_ELEMENT_API_STATE_VALID);
    writerBar2.m_tuples_[idx].m_text3.set( "123" );
    writerBar2.m_tuples_[idx].m_text5.set( "abcd" );
    writerBar2.addItem(idx);
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Updating ModelBar2...."));
    writerBar2.updateModel();

    Cpl::System::Api::sleep( 300 ); // Allow time for other threads to run
    recordLayer_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor.m_status == HealthRequest::eCLOSED );

    // Re-open the DB and Verify new values
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Verifying written values..." ));
    REQUIRE( recordLayer_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor.m_status == HealthRequest::eRUNNING );
    query1.issueQuery();
    display_bar1_( query1, "Query Bar1: Verify updated values..." );
    REQUIRE( query1.m_fields1.m_text1.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T1 );
    REQUIRE( query1.m_fields1.m_text3.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T3 );
    REQUIRE( query1.m_fields1.m_text8.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T8 );
    REQUIRE( query1.m_fields2.m_text10.isValid() == REC_BAR1_TUP2_DEFAULT_VALID_T10 );
    if ( query1.m_fields1.m_text1.isValid() )  { REQUIRE( query1.m_fields1.m_text1.getString()  == "*" ); }
    if ( query1.m_fields1.m_text3.isValid() )  { REQUIRE( query1.m_fields1.m_text3.getString()  == REC_BAR1_TUP1_DEFAULT_T3 ); }
    if ( query1.m_fields1.m_text8.isValid() )  { REQUIRE( query1.m_fields1.m_text8.getString()  == REC_BAR1_TUP1_DEFAULT_T8 ); }
    if ( query1.m_fields2.m_text10.isValid() ) { REQUIRE( query1.m_fields2.m_text10.getString() == "bobs not h" ); }

    query2.issueQuery();
    display_bar2_( query2, "Query Bar2: Verify updated values" );
    REQUIRE( query2[0].isInContainer() == true );
    if ( query2[0].isInContainer() )
        {
        REQUIRE( query2[0].m_text3.isValid() == true );
        REQUIRE( query2[0].m_text5.isValid() == true );
        if ( query2[0].m_text3.isValid() ) { REQUIRE( query2[0].m_text3.getString() == "123" ); }
        if ( query2[0].m_text5.isValid() ) { REQUIRE( query2[0].m_text5.getString() == "abcd" ); }
        }
    REQUIRE( query2[1].isInContainer() == REC_BAR2_TUP1_DEFAULT_INCONTAINER );
    if ( query2[1].isInContainer() )
        {
        REQUIRE( query2[1].m_text3.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T3 );
        REQUIRE( query2[1].m_text5.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T5 );
        if ( query2[1].m_text3.isValid() ) { REQUIRE( query2[1].m_text3.getString() == REC_BAR2_TUP1_DEFAULT_T3 ); }
        if ( query2[1].m_text5.isValid() ) { REQUIRE( query2[1].m_text5.getString() == REC_BAR2_TUP1_DEFAULT_T5 ); }
        }

    REQUIRE( query2[2].isInContainer() == REC_BAR2_TUP2_DEFAULT_INCONTAINER );
    if ( query2[2].isInContainer() )
        {
        REQUIRE( query2[2].m_text3.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T3 );
        REQUIRE( query2[2].m_text5.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T5 );
        if ( query2[2].m_text3.isValid() ) { REQUIRE( query2[2].m_text3.getString() == REC_BAR2_TUP2_DEFAULT_T3 ); }
        if ( query2[2].m_text5.isValid() ) { REQUIRE( query2[2].m_text5.getString() == REC_BAR2_TUP2_DEFAULT_T5 ); }
        }

    REQUIRE( query2[3].isInContainer() == REC_BAR2_TUP3_DEFAULT_INCONTAINER );
    if ( query2[3].isInContainer() )
        {
        REQUIRE( query2[3].m_text3.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T3 );
        REQUIRE( query2[3].m_text5.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T5 );
        if ( query2[3].m_text3.isValid() ) { REQUIRE( query2[3].m_text3.getString() == REC_BAR2_TUP3_DEFAULT_T3 ); }
        if ( query2[3].m_text5.isValid() ) { REQUIRE( query2[3].m_text5.getString() == REC_BAR2_TUP3_DEFAULT_T5 ); }
        }


    recordLayer_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor.m_status == HealthRequest::eCLOSED );
    REQUIRE( myMonitor.m_openingCount == 2 );
    REQUIRE( myMonitor.m_runningCount == 2 );
    REQUIRE( myMonitor.m_runningUpgradeCount == 0 );
    REQUIRE( myMonitor.m_runningCorruptCount == 0 );
    REQUIRE( myMonitor.m_errMediaCount == 0 );
    REQUIRE( myMonitor.m_errSchemaCount == 0 );
    REQUIRE( myMonitor.m_closingCount == 2 );
    REQUIRE( myMonitor.m_closedCount == 3 );
    myMonitor.close();


    //
    // Test: Incompatible database file
    //
    // Default empty DB to application defaults
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "" ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "################### Opening incompatible DB file..." ));
    myMonitor2.open();
    REQUIRE( recordLayer2_.open() == false );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor2.m_status == HealthRequest::eNO_STORAGE_WRONG_SCHEMA );

    QueryBar1 query1a(modelBar1a_);
    query1a.issueQuery();
    display_bar1_( query1a, "Query Bar1: Verify initial defaults" );
    REQUIRE( query1a.m_fields1.m_text1.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T1 );
    REQUIRE( query1a.m_fields1.m_text3.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T3 );
    REQUIRE( query1a.m_fields1.m_text8.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T8 );
    REQUIRE( query1a.m_fields2.m_text10.isValid() == REC_BAR1_TUP2_DEFAULT_VALID_T10 );
    if ( query1a.m_fields1.m_text1.isValid() )  { REQUIRE( query1a.m_fields1.m_text1.getString()  == REC_BAR1_TUP1_DEFAULT_T1 ); }
    if ( query1a.m_fields1.m_text3.isValid() )  { REQUIRE( query1a.m_fields1.m_text3.getString()  == REC_BAR1_TUP1_DEFAULT_T3 ); }
    if ( query1a.m_fields1.m_text8.isValid() )  { REQUIRE( query1a.m_fields1.m_text8.getString()  == REC_BAR1_TUP1_DEFAULT_T8 ); }
    if ( query1a.m_fields2.m_text10.isValid() ) { REQUIRE( query1a.m_fields2.m_text10.getString() == REC_BAR1_TUP2_DEFAULT_T10 ); }

    QueryBar2 query2a(modelBar2a_);
    query2a.issueQuery();
    display_bar2_( query2a, "Query Bar2: Verify initial defaults" );
    REQUIRE( query2a[0].isInContainer() == REC_BAR2_TUP0_DEFAULT_INCONTAINER );
    if ( query2a[0].isInContainer() )
        {
        REQUIRE( query2a[0].m_text3.isValid() == REC_BAR2_TUP0_DEFAULT_VALID_T3 );
        REQUIRE( query2a[0].m_text5.isValid() == REC_BAR2_TUP0_DEFAULT_VALID_T5 );
        if ( query2a[0].m_text3.isValid() ) { REQUIRE( query2a[0].m_text3.getString() == REC_BAR2_TUP0_DEFAULT_T3 ); }
        if ( query2a[0].m_text5.isValid() ) { REQUIRE( query2a[0].m_text5.getString() == REC_BAR2_TUP0_DEFAULT_T5 ); }
        }
    REQUIRE( query2a[1].isInContainer() == REC_BAR2_TUP1_DEFAULT_INCONTAINER );
    if ( query2a[1].isInContainer() )
        {
        REQUIRE( query2a[1].m_text3.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T3 );
        REQUIRE( query2a[1].m_text5.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T5 );
        if ( query2a[1].m_text3.isValid() ) { REQUIRE( query2a[1].m_text3.getString() == REC_BAR2_TUP1_DEFAULT_T3 ); }
        if ( query2a[1].m_text5.isValid() ) { REQUIRE( query2a[1].m_text5.getString() == REC_BAR2_TUP1_DEFAULT_T5 ); }
        }

    REQUIRE( query2a[2].isInContainer() == REC_BAR2_TUP2_DEFAULT_INCONTAINER );
    if ( query2a[2].isInContainer() )
        {
        REQUIRE( query2a[2].m_text3.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T3 );
        REQUIRE( query2a[2].m_text5.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T5 );
        if ( query2a[2].m_text3.isValid() ) { REQUIRE( query2a[2].m_text3.getString() == REC_BAR2_TUP2_DEFAULT_T3 ); }
        if ( query2a[2].m_text5.isValid() ) { REQUIRE( query2a[2].m_text5.getString() == REC_BAR2_TUP2_DEFAULT_T5 ); }
        }

    REQUIRE( query2a[3].isInContainer() == REC_BAR2_TUP3_DEFAULT_INCONTAINER );
    if ( query2a[3].isInContainer() )
        {
        REQUIRE( query2a[3].m_text3.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T3 );
        REQUIRE( query2a[3].m_text5.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T5 );
        if ( query2a[3].m_text3.isValid() ) { REQUIRE( query2a[3].m_text3.getString() == REC_BAR2_TUP3_DEFAULT_T3 ); }
        if ( query2a[3].m_text5.isValid() ) { REQUIRE( query2a[3].m_text5.getString() == REC_BAR2_TUP3_DEFAULT_T5 ); }
        }

    recordLayer2_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor2.m_status == HealthRequest::eCLOSED );
    
    // Re-open the DB and Verify that the original DB was NOT updated
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Verifying that the DB was not modified..." ));
    REQUIRE( recordLayer2_.open() == false );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor2.m_status == HealthRequest::eNO_STORAGE_WRONG_SCHEMA );


    recordLayer2_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor2.m_status == HealthRequest::eCLOSED );
    REQUIRE( myMonitor2.m_openingCount >= 1 );
    REQUIRE( myMonitor2.m_runningCount == 0 );
    REQUIRE( myMonitor2.m_runningUpgradeCount == 0 );
    REQUIRE( myMonitor2.m_runningCorruptCount == 0 );
    REQUIRE( myMonitor2.m_errMediaCount == 0 );
    REQUIRE( myMonitor2.m_errSchemaCount == 2 );
    REQUIRE( myMonitor2.m_closingCount == 2 );
    REQUIRE( myMonitor2.m_closedCount == 3 );
    myMonitor2.close();


    //
    // Test: File error on open
    //
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "" ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "################### Opening DB file - with Null Media, aka file open error test..." ));
    myMonitor3.open();
    REQUIRE( recordLayer3_.open() == false );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor3.m_status == HealthRequest::eNO_STORAGE_MEDIA_ERR );

    QueryBar1 query1b(modelBar1b_);
    query1b.issueQuery();
    display_bar1_( query1b, "Query Bar1: Verify initial defaults" );
    REQUIRE( query1b.m_fields1.m_text1.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T1 );
    REQUIRE( query1b.m_fields1.m_text3.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T3 );
    REQUIRE( query1b.m_fields1.m_text8.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T8 );
    REQUIRE( query1b.m_fields2.m_text10.isValid() == REC_BAR1_TUP2_DEFAULT_VALID_T10 );
    if ( query1b.m_fields1.m_text1.isValid() )  { REQUIRE( query1b.m_fields1.m_text1.getString()  == REC_BAR1_TUP1_DEFAULT_T1 ); }
    if ( query1b.m_fields1.m_text3.isValid() )  { REQUIRE( query1b.m_fields1.m_text3.getString()  == REC_BAR1_TUP1_DEFAULT_T3 ); }
    if ( query1b.m_fields1.m_text8.isValid() )  { REQUIRE( query1b.m_fields1.m_text8.getString()  == REC_BAR1_TUP1_DEFAULT_T8 ); }
    if ( query1b.m_fields2.m_text10.isValid() ) { REQUIRE( query1b.m_fields2.m_text10.getString() == REC_BAR1_TUP2_DEFAULT_T10 ); }

    QueryBar2 query2b(modelBar2b_);
    query2b.issueQuery();
    display_bar2_( query2b, "Query Bar2: Verify initial defaults" );
    REQUIRE( query2b[0].isInContainer() == REC_BAR2_TUP0_DEFAULT_INCONTAINER );
    if ( query2b[0].isInContainer() )
        {
        REQUIRE( query2b[0].m_text3.isValid() == REC_BAR2_TUP0_DEFAULT_VALID_T3 );
        REQUIRE( query2b[0].m_text5.isValid() == REC_BAR2_TUP0_DEFAULT_VALID_T5 );
        if ( query2b[0].m_text3.isValid() ) { REQUIRE( query2b[0].m_text3.getString() == REC_BAR2_TUP0_DEFAULT_T3 ); }
        if ( query2b[0].m_text5.isValid() ) { REQUIRE( query2b[0].m_text5.getString() == REC_BAR2_TUP0_DEFAULT_T5 ); }
        }
    REQUIRE( query2b[1].isInContainer() == REC_BAR2_TUP1_DEFAULT_INCONTAINER );
    if ( query2b[1].isInContainer() )
        {
        REQUIRE( query2b[1].m_text3.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T3 );
        REQUIRE( query2b[1].m_text5.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T5 );
        if ( query2b[1].m_text3.isValid() ) { REQUIRE( query2b[1].m_text3.getString() == REC_BAR2_TUP1_DEFAULT_T3 ); }
        if ( query2b[1].m_text5.isValid() ) { REQUIRE( query2b[1].m_text5.getString() == REC_BAR2_TUP1_DEFAULT_T5 ); }
        }

    REQUIRE( query2b[2].isInContainer() == REC_BAR2_TUP2_DEFAULT_INCONTAINER );
    if ( query2b[2].isInContainer() )
        {
        REQUIRE( query2b[2].m_text3.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T3 );
        REQUIRE( query2b[2].m_text5.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T5 );
        if ( query2b[2].m_text3.isValid() ) { REQUIRE( query2b[2].m_text3.getString() == REC_BAR2_TUP2_DEFAULT_T3 ); }
        if ( query2b[2].m_text5.isValid() ) { REQUIRE( query2b[2].m_text5.getString() == REC_BAR2_TUP2_DEFAULT_T5 ); }
        }

    REQUIRE( query2b[3].isInContainer() == REC_BAR2_TUP3_DEFAULT_INCONTAINER );
    if ( query2b[3].isInContainer() )
        {
        REQUIRE( query2b[3].m_text3.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T3 );
        REQUIRE( query2b[3].m_text5.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T5 );
        if ( query2b[3].m_text3.isValid() ) { REQUIRE( query2b[3].m_text3.getString() == REC_BAR2_TUP3_DEFAULT_T3 ); }
        if ( query2b[3].m_text5.isValid() ) { REQUIRE( query2b[3].m_text5.getString() == REC_BAR2_TUP3_DEFAULT_T5 ); }
        }

    recordLayer3_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor3.m_status == HealthRequest::eCLOSED );
    REQUIRE( myMonitor3.m_openingCount >= 0 );
    REQUIRE( myMonitor3.m_runningCount == 0 );
    REQUIRE( myMonitor3.m_runningUpgradeCount == 0 );
    REQUIRE( myMonitor3.m_runningCorruptCount == 0 );
    REQUIRE( myMonitor3.m_errMediaCount == 1 );
    REQUIRE( myMonitor3.m_errSchemaCount == 0 );
    REQUIRE( myMonitor3.m_closingCount == 1 );
    REQUIRE( myMonitor3.m_closedCount == 2 );
    myMonitor3.close();


    //
    // Test: Minor upgrade
    //
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "" ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "################### Opening DB file - minor upgrade (adding 2 tuples to Bar2)..." ));
    myMonitor4.open();
    REQUIRE( recordLayer4_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor4.m_status == HealthRequest::eRUNNING_MINOR_UPGRADE );
    recordLayer4_.close();
    Cpl::System::Api::sleep( 100 );

    // Re-open the DB and Verify new values
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Verifying written values..." ));
    REQUIRE( recordLayer4_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor4.m_status == HealthRequest::eRUNNING );

    QueryBar1 query1Ext(modelBar1Ext_);
    query1Ext.issueQuery();
    display_bar1_( query1Ext, "Query Bar1: Verify values..." );
    REQUIRE( query1Ext.m_fields1.m_text1.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T1 );
    REQUIRE( query1Ext.m_fields1.m_text3.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T3 );
    REQUIRE( query1Ext.m_fields1.m_text8.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T8 );
    REQUIRE( query1Ext.m_fields2.m_text10.isValid() == REC_BAR1_TUP2_DEFAULT_VALID_T10 );
    if ( query1Ext.m_fields1.m_text1.isValid() )  { REQUIRE( query1Ext.m_fields1.m_text1.getString()  == "*" ); }
    if ( query1Ext.m_fields1.m_text3.isValid() )  { REQUIRE( query1Ext.m_fields1.m_text3.getString()  == REC_BAR1_TUP1_DEFAULT_T3 ); }
    if ( query1Ext.m_fields1.m_text8.isValid() )  { REQUIRE( query1Ext.m_fields1.m_text8.getString()  == REC_BAR1_TUP1_DEFAULT_T8 ); }
    if ( query1Ext.m_fields2.m_text10.isValid() ) { REQUIRE( query1Ext.m_fields2.m_text10.getString() == "bobs not h" ); }

    
    QueryBar2Ext query2Ext(modelBar2Ext_);
    query2Ext.issueQuery();
    display_bar2ext_( query2Ext, "Query Bar2: Verify values" );
    REQUIRE( query2Ext[0].isInContainer() == true );
    if ( query2Ext[0].isInContainer() )
        {
        REQUIRE( query2Ext[0].m_text3.isValid() == true );
        REQUIRE( query2Ext[0].m_text5.isValid() == true );
        if ( query2Ext[0].m_text3.isValid() ) { REQUIRE( query2Ext[0].m_text3.getString() == "123" ); }
        if ( query2Ext[0].m_text5.isValid() ) { REQUIRE( query2Ext[0].m_text5.getString() == "abcd" ); }
        }
    REQUIRE( query2Ext[1].isInContainer() == REC_BAR2_TUP1_DEFAULT_INCONTAINER );
    if ( query2Ext[1].isInContainer() )
        {
        REQUIRE( query2Ext[1].m_text3.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[1].m_text5.isValid() == REC_BAR2_TUP1_DEFAULT_VALID_T5 );
        if ( query2Ext[1].m_text3.isValid() ) { REQUIRE( query2Ext[1].m_text3.getString() == REC_BAR2_TUP1_DEFAULT_T3 ); }
        if ( query2Ext[1].m_text5.isValid() ) { REQUIRE( query2Ext[1].m_text5.getString() == REC_BAR2_TUP1_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[2].isInContainer() == REC_BAR2_TUP2_DEFAULT_INCONTAINER );
    if ( query2Ext[2].isInContainer() )
        {
        REQUIRE( query2Ext[2].m_text3.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[2].m_text5.isValid() == REC_BAR2_TUP2_DEFAULT_VALID_T5 );
        if ( query2Ext[2].m_text3.isValid() ) { REQUIRE( query2Ext[2].m_text3.getString() == REC_BAR2_TUP2_DEFAULT_T3 ); }
        if ( query2Ext[2].m_text5.isValid() ) { REQUIRE( query2Ext[2].m_text5.getString() == REC_BAR2_TUP2_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[3].isInContainer() == REC_BAR2_TUP3_DEFAULT_INCONTAINER );
    if ( query2Ext[3].isInContainer() )
        {
        REQUIRE( query2Ext[3].m_text3.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[3].m_text5.isValid() == REC_BAR2_TUP3_DEFAULT_VALID_T5 );
        if ( query2Ext[3].m_text3.isValid() ) { REQUIRE( query2Ext[3].m_text3.getString() == REC_BAR2_TUP3_DEFAULT_T3 ); }
        if ( query2Ext[3].m_text5.isValid() ) { REQUIRE( query2Ext[3].m_text5.getString() == REC_BAR2_TUP3_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[4].isInContainer() == REC_BAR2EXT_TUP4_DEFAULT_INCONTAINER );
    if ( query2Ext[4].isInContainer() )
        {
        REQUIRE( query2Ext[4].m_text3.isValid() == REC_BAR2EXT_TUP4_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[4].m_text5.isValid() == REC_BAR2EXT_TUP4_DEFAULT_VALID_T5 );
        if ( query2Ext[4].m_text3.isValid() ) { REQUIRE( query2Ext[4].m_text3.getString() == REC_BAR2EXT_TUP4_DEFAULT_T3 ); }
        if ( query2Ext[4].m_text5.isValid() ) { REQUIRE( query2Ext[4].m_text5.getString() == REC_BAR2EXT_TUP4_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[5].isInContainer() == REC_BAR2EXT_TUP5_DEFAULT_INCONTAINER );
    if ( query2Ext[5].isInContainer() )
        {
        REQUIRE( query2Ext[5].m_text3.isValid() == REC_BAR2EXT_TUP5_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[5].m_text5.isValid() == REC_BAR2EXT_TUP5_DEFAULT_VALID_T5 );
        if ( query2Ext[5].m_text3.isValid() ) { REQUIRE( query2Ext[5].m_text3.getString() == REC_BAR2EXT_TUP5_DEFAULT_T3 ); }
        if ( query2Ext[5].m_text5.isValid() ) { REQUIRE( query2Ext[5].m_text5.getString() == REC_BAR2EXT_TUP5_DEFAULT_T5 ); }
        }

    recordLayer4_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor4.m_status == HealthRequest::eCLOSED );
    REQUIRE( myMonitor4.m_openingCount == 2 );
    REQUIRE( myMonitor4.m_runningCount == 1 );
    REQUIRE( myMonitor4.m_runningUpgradeCount == 1 );
    REQUIRE( myMonitor4.m_runningCorruptCount == 0 );
    REQUIRE( myMonitor4.m_errMediaCount == 0 );
    REQUIRE( myMonitor4.m_errSchemaCount == 0 );
    REQUIRE( myMonitor4.m_closingCount == 2 );
    REQUIRE( myMonitor4.m_closedCount == 3 );
    myMonitor4.close();


    //
    // TEST: Corrupt a Record
    //
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "" ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "################### Testing corrupt DB file..." ));
    Cpl::Io::File::Output dbfile( DB_FNAME_, false );
    REQUIRE( dbfile.isOpened() );
    REQUIRE( dbfile.setAbsolutePos( 0x50 ) );  // NOTE: Should be corrupting the BAR2Ext record
    REQUIRE( dbfile.write( '*' ) );
    dbfile.close();
     
    myMonitor4.open();
    REQUIRE( recordLayer4_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor4.m_status == HealthRequest::eRUNNING_CORRUPTED_INPUT );
    recordLayer4_.close();
    Cpl::System::Api::sleep( 100 );

    // Re-open the DB and Verify new values
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Verifying written values..." ));
    REQUIRE( recordLayer4_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor4.m_status == HealthRequest::eRUNNING );

    query1Ext.issueQuery();
    display_bar1_( query1Ext, "Query Bar1: Verify values..." );
    REQUIRE( query1Ext.m_fields1.m_text1.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T1 );
    REQUIRE( query1Ext.m_fields1.m_text3.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T3 );
    REQUIRE( query1Ext.m_fields1.m_text8.isValid()  == REC_BAR1_TUP1_DEFAULT_VALID_T8 );
    REQUIRE( query1Ext.m_fields2.m_text10.isValid() == REC_BAR1_TUP2_DEFAULT_VALID_T10 );
    if ( query1Ext.m_fields1.m_text1.isValid() )  { REQUIRE( query1Ext.m_fields1.m_text1.getString()  == "*" ); }
    if ( query1Ext.m_fields1.m_text3.isValid() )  { REQUIRE( query1Ext.m_fields1.m_text3.getString()  == REC_BAR1_TUP1_DEFAULT_T3 ); }
    if ( query1Ext.m_fields1.m_text8.isValid() )  { REQUIRE( query1Ext.m_fields1.m_text8.getString()  == REC_BAR1_TUP1_DEFAULT_T8 ); }
    if ( query1Ext.m_fields2.m_text10.isValid() ) { REQUIRE( query1Ext.m_fields2.m_text10.getString() == "bobs not h" ); }

    
    query2Ext.issueQuery();
    display_bar2ext_( query2Ext, "Query Bar2: Verify values" );
    REQUIRE( query2Ext[0].isInContainer() == REC_BAR2EXT_TUP0_DEFAULT_INCONTAINER );
    if ( query2Ext[0].isInContainer() )
        {
        REQUIRE( query2Ext[0].m_text3.isValid() == REC_BAR2EXT_TUP0_DEFAULT_VALID_T3);
        REQUIRE( query2Ext[0].m_text5.isValid() == REC_BAR2EXT_TUP0_DEFAULT_VALID_T5);
        if ( query2Ext[0].m_text3.isValid() ) { REQUIRE( query2Ext[0].m_text3.getString() == REC_BAR2EXT_TUP0_DEFAULT_T3 ); }
        if ( query2Ext[0].m_text5.isValid() ) { REQUIRE( query2Ext[0].m_text5.getString() == REC_BAR2EXT_TUP0_DEFAULT_T5 ); }
        }
    REQUIRE( query2Ext[1].isInContainer() == REC_BAR2EXT_TUP1_DEFAULT_INCONTAINER );
    if ( query2Ext[1].isInContainer() )
        {
        REQUIRE( query2Ext[1].m_text3.isValid() == REC_BAR2EXT_TUP1_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[1].m_text5.isValid() == REC_BAR2EXT_TUP1_DEFAULT_VALID_T5 );
        if ( query2Ext[1].m_text3.isValid() ) { REQUIRE( query2Ext[1].m_text3.getString() == REC_BAR2EXT_TUP1_DEFAULT_T3 ); }
        if ( query2Ext[1].m_text5.isValid() ) { REQUIRE( query2Ext[1].m_text5.getString() == REC_BAR2EXT_TUP1_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[2].isInContainer() == REC_BAR2EXT_TUP2_DEFAULT_INCONTAINER );
    if ( query2Ext[2].isInContainer() )
        {
        REQUIRE( query2Ext[2].m_text3.isValid() == REC_BAR2EXT_TUP2_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[2].m_text5.isValid() == REC_BAR2EXT_TUP2_DEFAULT_VALID_T5 );
        if ( query2Ext[2].m_text3.isValid() ) { REQUIRE( query2Ext[2].m_text3.getString() == REC_BAR2EXT_TUP2_DEFAULT_T3 ); }
        if ( query2Ext[2].m_text5.isValid() ) { REQUIRE( query2Ext[2].m_text5.getString() == REC_BAR2EXT_TUP2_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[3].isInContainer() == REC_BAR2EXT_TUP3_DEFAULT_INCONTAINER );
    if ( query2Ext[3].isInContainer() )
        {
        REQUIRE( query2Ext[3].m_text3.isValid() == REC_BAR2EXT_TUP3_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[3].m_text5.isValid() == REC_BAR2EXT_TUP3_DEFAULT_VALID_T5 );
        if ( query2Ext[3].m_text3.isValid() ) { REQUIRE( query2Ext[3].m_text3.getString() == REC_BAR2EXT_TUP3_DEFAULT_T3 ); }
        if ( query2Ext[3].m_text5.isValid() ) { REQUIRE( query2Ext[3].m_text5.getString() == REC_BAR2EXT_TUP3_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[4].isInContainer() == REC_BAR2EXT_TUP4_DEFAULT_INCONTAINER );
    if ( query2Ext[4].isInContainer() )
        {
        REQUIRE( query2Ext[4].m_text3.isValid() == REC_BAR2EXT_TUP4_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[4].m_text5.isValid() == REC_BAR2EXT_TUP4_DEFAULT_VALID_T5 );
        if ( query2Ext[4].m_text3.isValid() ) { REQUIRE( query2Ext[4].m_text3.getString() == REC_BAR2EXT_TUP4_DEFAULT_T3 ); }
        if ( query2Ext[4].m_text5.isValid() ) { REQUIRE( query2Ext[4].m_text5.getString() == REC_BAR2EXT_TUP4_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[5].isInContainer() == REC_BAR2EXT_TUP5_DEFAULT_INCONTAINER );
    if ( query2Ext[5].isInContainer() )
        {
        REQUIRE( query2Ext[5].m_text3.isValid() == REC_BAR2EXT_TUP5_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[5].m_text5.isValid() == REC_BAR2EXT_TUP5_DEFAULT_VALID_T5 );
        if ( query2Ext[5].m_text3.isValid() ) { REQUIRE( query2Ext[5].m_text3.getString() == REC_BAR2EXT_TUP5_DEFAULT_T3 ); }
        if ( query2Ext[5].m_text5.isValid() ) { REQUIRE( query2Ext[5].m_text5.getString() == REC_BAR2EXT_TUP5_DEFAULT_T5 ); }
        }

    recordLayer4_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor4.m_status == HealthRequest::eCLOSED );
    REQUIRE( myMonitor4.m_openingCount == 2 );
    REQUIRE( myMonitor4.m_runningCount == 1 );
    REQUIRE( myMonitor4.m_runningUpgradeCount == 0 );
    REQUIRE( myMonitor4.m_runningCorruptCount == 1 );
    REQUIRE( myMonitor4.m_errMediaCount == 0 );
    REQUIRE( myMonitor4.m_errSchemaCount == 0 );
    REQUIRE( myMonitor4.m_closingCount == 2 );
    REQUIRE( myMonitor4.m_closedCount == 3 );
    myMonitor4.close();


    //
    // Test: Minor upgrade - purge a record
    //
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "" ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "################### Opening DB file - minor upgrade - purging Bar1..." ));
    recordList4_.remove( modelBar1Ext_.getKey() );
    myMonitor4.open();
    REQUIRE( recordLayer4_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor4.m_status == HealthRequest::eRUNNING_MINOR_UPGRADE );

    ControllerBar2Ext writerBar2Ext( modelBar2Ext_ );
    idx = 1;
    writerBar2Ext.m_tuples_[idx].setAllInUseState(true);
    writerBar2Ext.m_tuples_[idx].setAllValidState(RTE_ELEMENT_API_STATE_VALID);
    writerBar2Ext.m_tuples_[idx].m_text3.set( "223" );
    writerBar2Ext.m_tuples_[idx].m_text5.set( "xyz" );
    writerBar2Ext.addItem(idx);
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Updating ModelBar2Ext...."));
    writerBar2Ext.updateModel();
    Cpl::System::Api::sleep( 300 );

    recordLayer4_.close();
    Cpl::System::Api::sleep( 100 );

    // Re-open the DB and Verify new values
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Verifying written values..." ));
    REQUIRE( recordLayer4_.open() );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( myMonitor4.m_status == HealthRequest::eRUNNING );

    query2Ext.issueQuery();
    display_bar2ext_( query2Ext, "Query Bar2: Verify values" );
    REQUIRE( query2Ext[0].isInContainer() == REC_BAR2EXT_TUP0_DEFAULT_INCONTAINER );
    if ( query2Ext[0].isInContainer() )
        {
        REQUIRE( query2Ext[0].m_text3.isValid() == REC_BAR2EXT_TUP0_DEFAULT_VALID_T3);
        REQUIRE( query2Ext[0].m_text5.isValid() == REC_BAR2EXT_TUP0_DEFAULT_VALID_T5);
        if ( query2Ext[0].m_text3.isValid() ) { REQUIRE( query2Ext[0].m_text3.getString() == REC_BAR2EXT_TUP0_DEFAULT_T3 ); }
        if ( query2Ext[0].m_text5.isValid() ) { REQUIRE( query2Ext[0].m_text5.getString() == REC_BAR2EXT_TUP0_DEFAULT_T5 ); }
        }
    REQUIRE( query2Ext[1].isInContainer() == true );
    if ( query2Ext[1].isInContainer() )
        {
        REQUIRE( query2Ext[1].m_text3.isValid() == true );
        REQUIRE( query2Ext[1].m_text5.isValid() == true );
        if ( query2Ext[1].m_text3.isValid() ) { REQUIRE( query2Ext[1].m_text3.getString() == "223" ); }
        if ( query2Ext[1].m_text5.isValid() ) { REQUIRE( query2Ext[1].m_text5.getString() == "xyz" ); }
        }

    REQUIRE( query2Ext[2].isInContainer() == REC_BAR2EXT_TUP2_DEFAULT_INCONTAINER );
    if ( query2Ext[2].isInContainer() )
        {
        REQUIRE( query2Ext[2].m_text3.isValid() == REC_BAR2EXT_TUP2_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[2].m_text5.isValid() == REC_BAR2EXT_TUP2_DEFAULT_VALID_T5 );
        if ( query2Ext[2].m_text3.isValid() ) { REQUIRE( query2Ext[2].m_text3.getString() == REC_BAR2EXT_TUP2_DEFAULT_T3 ); }
        if ( query2Ext[2].m_text5.isValid() ) { REQUIRE( query2Ext[2].m_text5.getString() == REC_BAR2EXT_TUP2_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[3].isInContainer() == REC_BAR2EXT_TUP3_DEFAULT_INCONTAINER );
    if ( query2Ext[3].isInContainer() )
        {
        REQUIRE( query2Ext[3].m_text3.isValid() == REC_BAR2EXT_TUP3_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[3].m_text5.isValid() == REC_BAR2EXT_TUP3_DEFAULT_VALID_T5 );
        if ( query2Ext[3].m_text3.isValid() ) { REQUIRE( query2Ext[3].m_text3.getString() == REC_BAR2EXT_TUP3_DEFAULT_T3 ); }
        if ( query2Ext[3].m_text5.isValid() ) { REQUIRE( query2Ext[3].m_text5.getString() == REC_BAR2EXT_TUP3_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[4].isInContainer() == REC_BAR2EXT_TUP4_DEFAULT_INCONTAINER );
    if ( query2Ext[4].isInContainer() )
        {
        REQUIRE( query2Ext[4].m_text3.isValid() == REC_BAR2EXT_TUP4_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[4].m_text5.isValid() == REC_BAR2EXT_TUP4_DEFAULT_VALID_T5 );
        if ( query2Ext[4].m_text3.isValid() ) { REQUIRE( query2Ext[4].m_text3.getString() == REC_BAR2EXT_TUP4_DEFAULT_T3 ); }
        if ( query2Ext[4].m_text5.isValid() ) { REQUIRE( query2Ext[4].m_text5.getString() == REC_BAR2EXT_TUP4_DEFAULT_T5 ); }
        }

    REQUIRE( query2Ext[5].isInContainer() == REC_BAR2EXT_TUP5_DEFAULT_INCONTAINER );
    if ( query2Ext[5].isInContainer() )
        {
        REQUIRE( query2Ext[5].m_text3.isValid() == REC_BAR2EXT_TUP5_DEFAULT_VALID_T3 );
        REQUIRE( query2Ext[5].m_text5.isValid() == REC_BAR2EXT_TUP5_DEFAULT_VALID_T5 );
        if ( query2Ext[5].m_text3.isValid() ) { REQUIRE( query2Ext[5].m_text3.getString() == REC_BAR2EXT_TUP5_DEFAULT_T3 ); }
        if ( query2Ext[5].m_text5.isValid() ) { REQUIRE( query2Ext[5].m_text5.getString() == REC_BAR2EXT_TUP5_DEFAULT_T5 ); }
        }

    recordLayer4_.close();
    Cpl::System::Api::sleep( 100 );
    REQUIRE( myMonitor4.m_status == HealthRequest::eCLOSED );
    REQUIRE( myMonitor4.m_openingCount == 2 );
    REQUIRE( myMonitor4.m_runningCount == 1 );
    REQUIRE( myMonitor4.m_runningUpgradeCount == 1 );
    REQUIRE( myMonitor4.m_runningCorruptCount == 0 );
    REQUIRE( myMonitor4.m_errMediaCount == 0 );
    REQUIRE( myMonitor4.m_errSchemaCount == 0 );
    REQUIRE( myMonitor4.m_closingCount == 2 );
    REQUIRE( myMonitor4.m_closedCount == 3 );
    myMonitor4.close();



    // Shutdown threads
    chunkMailbox_.pleaseStop();
    recordLayerMbox_.pleaseStop();
    monitorMbox_.pleaseStop();
    Cpl::System::Api::sleep(100);       // allow time for threads to stop
    REQUIRE( chunkThreadPtr->isRunning() == false );
    REQUIRE( recordLayerThreadPtr->isRunning() == false );
    REQUIRE( monitorThreadPtr->isRunning() == false );
    Cpl::System::Thread::destroy( *chunkThreadPtr );
    Cpl::System::Thread::destroy( *recordLayerThreadPtr );
    Cpl::System::Thread::destroy( *monitorThreadPtr );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0 );
    }
