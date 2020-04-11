/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Checksum/Crc32EthernetFast.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Persistence/Record/Server.h"
#include "Cpl/Dm/Persistence/Chunk/FileSystem.h"
#include "Cpl/Dm/Persistence/Chunk/Server.h"
#include "Cpl/Io/File/Api.h"
#include "Cpl/Io/File/Output.h"

#include "common.h"
#include <string.h>
#include <stdint.h>



////////////////////////////////////////////////////////////////////////////////
#define MP_MAX_LEN              64
#define MP_EXPORTED_OVERHEAD    6
#define MP_NAME_MAX_LEN         32
#define REC_NAME_MAX_LEN        16
#define REC_OVERHEAD            (4 + 4 + 2 + REC_NAME_MAX_LEN)
#define MAX_BUF_SIZE_           ((MP_MAX_LEN + MP_NAME_MAX_LEN + 2 + MP_EXPORTED_OVERHEAD)* 3 + REC_OVERHEAD)
#define MEDIA_FNAME_            "myMediaFile"
#define SIGNATURE_              "12346789"
#define SIGNATURE_SIZE_         (4+ 2+8 +4) // clen + nlen + strlen(SIGNATURE_) + ccrc

// Allocate/create my Model Database
static ModelDatabase                            modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Cpl::Dm::StaticInfo                     info_mp_recordServerStatus_( "RecordServerStatus" );
static Persistence::Record::MpServerStatus     mp_recordServerStatus_( modelDb_, info_mp_recordServerStatus_ );
static Cpl::Dm::StaticInfo                     info_mp_recordDefaultActionRequest_( "RecordServerDefault" );
static Mp::String                              mp_recordDefaultActionRequest_( modelDb_, info_mp_recordDefaultActionRequest_, MP_NAME_MAX_LEN );


static Cpl::Dm::StaticInfo                     info_mp_rec1Mp1_( "Rec1Mp1" );
static Cpl::Dm::StaticInfo                     info_mp_rec1Mp2_( "Rec1Mp2" );
static Cpl::Dm::StaticInfo                     info_mp_rec1Mp3_( "Rec1Mp3" );
static Mp::String                              mp_rec1Mp1_( modelDb_, info_mp_rec1Mp1_, MP_MAX_LEN );
static Mp::String                              mp_rec1Mp2_( modelDb_, info_mp_rec1Mp2_, MP_MAX_LEN );
static Mp::String                              mp_rec1Mp3_( modelDb_, info_mp_rec1Mp3_, MP_MAX_LEN );

static Cpl::Dm::StaticInfo                     info_mp_rec2Mp1_( "Rec2Mp1" );
static Cpl::Dm::StaticInfo                     info_mp_rec2Mp2_( "Rec2Mp2" );
static Cpl::Dm::StaticInfo                     info_mp_rec2Mp3_( "Rec2Mp3" );
static Mp::String                              mp_rec2Mp1_( modelDb_, info_mp_rec2Mp1_, MP_MAX_LEN );
static Mp::String                              mp_rec2Mp2_( modelDb_, info_mp_rec2Mp2_, MP_MAX_LEN );
static Mp::String                              mp_rec2Mp3_( modelDb_, info_mp_rec2Mp3_, MP_MAX_LEN );

static Cpl::Dm::StaticInfo                     info_mp_rec3Mp1_( "Rec3Mp1" );
static Cpl::Dm::StaticInfo                     info_mp_rec3Mp2_( "Rec3Mp2" );
static Cpl::Dm::StaticInfo                     info_mp_rec3Mp3_( "Rec3Mp3" );
static Mp::String                              mp_rec3Mp1_( modelDb_, info_mp_rec3Mp1_, MP_MAX_LEN );
static Mp::String                              mp_rec3Mp2_( modelDb_, info_mp_rec3Mp2_, MP_MAX_LEN );
static Mp::String                              mp_rec3Mp3_( modelDb_, info_mp_rec3Mp3_, MP_MAX_LEN );

static Cpl::Dm::StaticInfo                     info_mp_rec4Mp1_( "Rec4Mp1" );
static Cpl::Dm::StaticInfo                     info_mp_rec4Mp2_( "Rec4Mp2" );
static Cpl::Dm::StaticInfo                     info_mp_rec4Mp3_( "Rec4Mp3" );
static Mp::String                              mp_rec4Mp1_( modelDb_, info_mp_rec4Mp1_, MP_MAX_LEN );
static Mp::String                              mp_rec4Mp2_( modelDb_, info_mp_rec4Mp2_, MP_MAX_LEN );
static Mp::String                              mp_rec4Mp3_( modelDb_, info_mp_rec4Mp3_, MP_MAX_LEN );

static Cpl::Checksum::Crc32EthernetFast         chunkCrc_;
static Persistence::Chunk::FileSystem           mediaFile( MEDIA_FNAME_ );
static Persistence::Chunk::Server               chunkServer_( mediaFile, chunkCrc_, SIGNATURE_ );
static Cpl::Dm::MailboxServer                   chunkMailbox_;
static Persistence::Chunk::Request::SAP         chunkSap_( chunkServer_, chunkMailbox_ );

static Persistence::Chunk::Server               chunk2Server_( mediaFile, chunkCrc_, SIGNATURE_ "X" );
static Persistence::Chunk::Request::SAP         chunk2Sap_( chunk2Server_, chunkMailbox_ );

static MyNullMedia                              nullMediaFile;
static Persistence::Chunk::Server               chunk3Server_( nullMediaFile, chunkCrc_, SIGNATURE_ );
static Persistence::Chunk::Request::SAP         chunk3Sap_( chunk3Server_, chunkMailbox_ );

static uint8_t                                          recordLayerbuffer_[MAX_BUF_SIZE_];
static Cpl::Container::Map<Persistence::Record::Api_>   recordList_;
static Cpl::Dm::MailboxServer                           recordServerMailbox_;
static Persistence::Record::Server                      recordServer_( recordLayerbuffer_,
                                                                       sizeof( recordLayerbuffer_ ),
                                                                       chunkSap_,
                                                                       recordServerMailbox_,
                                                                       recordList_,
                                                                       mp_recordServerStatus_,
                                                                       mp_recordDefaultActionRequest_ );
static Persistence::Record::Server                      record2Server_( recordLayerbuffer_,
                                                                        sizeof( recordLayerbuffer_ ),
                                                                        chunk2Sap_,
                                                                        recordServerMailbox_,
                                                                        recordList_,
                                                                        mp_recordServerStatus_,
                                                                        mp_recordDefaultActionRequest_ );
static Persistence::Record::Server                      record3Server_( recordLayerbuffer_,
                                                                        sizeof( recordLayerbuffer_ ),
                                                                        chunk3Sap_,
                                                                        recordServerMailbox_,
                                                                        recordList_,
                                                                        mp_recordServerStatus_,
                                                                        mp_recordDefaultActionRequest_ );
static Cpl::Container::Map<Persistence::Record::Api_>   record4List_;
static Persistence::Record::Server                      record4Server_( recordLayerbuffer_,
                                                                        sizeof( recordLayerbuffer_ ),
                                                                        chunkSap_,
                                                                        recordServerMailbox_,
                                                                        record4List_,
                                                                        mp_recordServerStatus_,
                                                                        mp_recordDefaultActionRequest_ );
static MyRecord                                 record1_( recordList_, 0, "Record1", recordServerMailbox_, mp_rec1Mp1_, mp_rec1Mp2_, mp_rec1Mp3_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" );
static MyRecord                                 record2_( recordList_, 0, "Record2", recordServerMailbox_, mp_rec2Mp1_, mp_rec2Mp2_, mp_rec2Mp3_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" );
static MyRecord                                 record3_( recordList_, 0, "Record3", recordServerMailbox_, mp_rec3Mp1_, mp_rec3Mp2_, mp_rec3Mp3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" );

static MyRecord                                 record41_( record4List_, 0, "Record1", recordServerMailbox_, mp_rec1Mp1_, mp_rec1Mp2_, mp_rec1Mp3_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" );
static MyRecord                                 record42_( record4List_, 0, "Record2", recordServerMailbox_, mp_rec2Mp1_, mp_rec2Mp2_, mp_rec2Mp3_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" );
static MyRecord                                 record43_( record4List_, 0, "Record3", recordServerMailbox_, mp_rec3Mp1_, mp_rec3Mp2_, mp_rec3Mp3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" );
static MyRecord                                 record44_( record4List_, 0, "Record4", recordServerMailbox_, mp_rec4Mp1_, mp_rec4Mp2_, mp_rec4Mp3_, "Rec4Mp1Default", "Rec4Mp2Default", "Rec4Mp3Default" );

////////////////////////////////////////////////////////////////////////////////
static void invalidate_all_records_mps( void )
{
    mp_rec1Mp1_.setInvalid();
    mp_rec1Mp2_.setInvalid();
    mp_rec1Mp3_.setInvalid();

    mp_rec2Mp1_.setInvalid();
    mp_rec2Mp2_.setInvalid();
    mp_rec2Mp3_.setInvalid();

    mp_rec3Mp1_.setInvalid();
    mp_rec3Mp2_.setInvalid();
    mp_rec3Mp3_.setInvalid();
}

static bool are_all_records_mps_valid( void )
{
    if ( mp_rec1Mp1_.isNotValid() ) return false;
    if ( mp_rec1Mp2_.isNotValid() ) return false;
    if ( mp_rec1Mp3_.isNotValid() ) return false;

    if ( mp_rec2Mp1_.isNotValid() ) return false;
    if ( mp_rec2Mp2_.isNotValid() ) return false;
    if ( mp_rec2Mp3_.isNotValid() ) return false;

    if ( mp_rec3Mp1_.isNotValid() ) return false;
    if ( mp_rec3Mp2_.isNotValid() ) return false;
    if ( mp_rec3Mp3_.isNotValid() ) return false;
    return true;
}

static bool are_record_values_match( MyRecord& record, const char* mp1Value, const char* mp2Value, const char* mp3Value )
{
    Cpl::Text::FString<MP_MAX_LEN> value;
    if ( Cpl::Dm::ModelPoint::IS_VALID( record.m_mp1.read( value ) ) == false || value != mp1Value ) return false;
    if ( Cpl::Dm::ModelPoint::IS_VALID( record.m_mp2.read( value ) ) == false || value != mp2Value ) return false;
    if ( Cpl::Dm::ModelPoint::IS_VALID( record.m_mp3.read( value ) ) == false || value != mp3Value ) return false;
    return true;
}

static Cpl::System::Thread* t1_;
static Cpl::System::Thread* t2_;

TEST_CASE( "recordserver-open", "[recordserver-open]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-OPEN test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //
    // Open EMPTY FILE
    //

    // Ensure starting conditions
    Cpl::Io::File::Api::remove( MEDIA_FNAME_ );
    invalidate_all_records_mps();

    t1_ = Cpl::System::Thread::create( chunkMailbox_, "Chunk" );
    t2_ = Cpl::System::Thread::create( recordServerMailbox_, "Record" );

    REQUIRE( are_all_records_mps_valid() == false );
    REQUIRE( mp_recordServerStatus_.isNotValid() == true );

    // Load the records
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    Persistence::Record::ServerStatus serverStatus = Persistence::Record::ServerStatus::eUNKNOWN;
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );
    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    //
    // Open Existing file
    //

    // Load the records
    invalidate_all_records_mps();
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );
    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "recordserver-write", "[recordserver-write]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-WRITE test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Starting with an existing file - with default values

    // Ensure starting conditions
    invalidate_all_records_mps();

    REQUIRE( are_all_records_mps_valid() == false );

    // Load the records
    recordServer_.open();
    Persistence::Record::ServerStatus serverStatus = Persistence::Record::ServerStatus::eUNKNOWN;
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    Cpl::System::Api::sleep( 500 );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );

    // Update records...
    record1_.m_mp1.write( "BOB-Rec1Mp1" );
    record1_.m_mp2.write( "BOB-Rec1Mp2" );
    record3_.m_mp3.write( "BOB-Rec3Mp3" );
    Cpl::System::Api::sleep( 1000 );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );

    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    //
    // Open Existing file - with updated values
    //

    // Load the records
    invalidate_all_records_mps();
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );
    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


TEST_CASE( "recordserver-incompatible-db" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "recordserver-incompatible-db" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //
    // Open Existing file - with updated values - BUT with incorrect signature
    //

    // Load the records
    invalidate_all_records_mps();
    record2Server_.open();
    Cpl::System::Api::sleep( 500 );
    Persistence::Record::ServerStatus serverStatus = Persistence::Record::ServerStatus::eUNKNOWN;
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eNO_STORAGE_WRONG_SCHEMA );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );

    record2Server_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    //
    // Open Existing file - with updated values - WITH correct signature
    //

    // Load the records
    invalidate_all_records_mps();
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );
    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


TEST_CASE( "recordserver-open-file-error" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "recordserver-open-file-error" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //
    // Open Existing file - with updated values - BUT with 'bad' file descriptor
    //

    // Load the records
    invalidate_all_records_mps();
    REQUIRE( record3Server_.open() == false );
    Cpl::System::Api::sleep( 500 );
    Persistence::Record::ServerStatus serverStatus = Persistence::Record::ServerStatus::eUNKNOWN;
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eNO_STORAGE_MEDIA_ERR );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );

    record3Server_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    //
    // Open Existing file - with updated values - WITH correct signature
    //

    // Load the records
    invalidate_all_records_mps();
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );
    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


TEST_CASE( "recordserver-minor-upgrade-add-a-record" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "recordserver-minor-upgrade-add-a-record" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //
    // Open Existing file - with updated values
    //

    // Load the records
    invalidate_all_records_mps();
    REQUIRE( record4Server_.open() == true );
    Cpl::System::Api::sleep( 500 );
    Persistence::Record::ServerStatus serverStatus = Persistence::Record::ServerStatus::eUNKNOWN;
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING_MINOR_UPGRADE );
    REQUIRE( are_record_values_match( record41_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record42_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record43_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );
    REQUIRE( are_record_values_match( record44_, "Rec4Mp1Default", "Rec4Mp2Default", "Rec4Mp3Default" ) );
    record4Server_.close();

    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    //
    // Open Existing file - with updated values - WITH correct signature
    //

    // Load the records
    invalidate_all_records_mps();
    record4Server_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record41_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record42_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record43_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );
    REQUIRE( are_record_values_match( record44_, "Rec4Mp1Default", "Rec4Mp2Default", "Rec4Mp3Default" ) );
    record4Server_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "recordserver-minor-upgrade-delete-a-record" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "recordserver-minor-upgrade-delete-a-record" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //
    // Open Existing file - with updated values
    //

    // Load the records
    invalidate_all_records_mps();
    REQUIRE( recordServer_.open() == true );
    Cpl::System::Api::sleep( 500 );
    Persistence::Record::ServerStatus serverStatus = Persistence::Record::ServerStatus::eUNKNOWN;
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING_MINOR_UPGRADE );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );
    recordServer_.close();

    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    //
    // Open Existing file - with updated values - WITH correct signature
    //

    // Load the records
    invalidate_all_records_mps();
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );
    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "recordserver-corrupt-record" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "recordserver-corrupt-record" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Corrupt the DB file
    Cpl::Io::File::Output dbfile( MEDIA_FNAME_, false );
    REQUIRE( dbfile.isOpened() );
    REQUIRE( dbfile.setAbsolutePos( 0x50 ) );
    REQUIRE( dbfile.write( '*' ) );
    dbfile.close();

    //
    // Open Existing file - with updated values
    //

    // Load the records
    invalidate_all_records_mps();
    REQUIRE( recordServer_.open() == true );
    Cpl::System::Api::sleep( 500 );
    Persistence::Record::ServerStatus serverStatus = Persistence::Record::ServerStatus::eUNKNOWN;
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING_CORRUPTED_INPUT );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );
    recordServer_.close();

    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    //
    // Open Existing file - with defaulted values 
    //

    // Load the records
    invalidate_all_records_mps();
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );
    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "recordserver-default-records")
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "recordserver-default-records" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Ensure starting conditions
    Cpl::Io::File::Api::remove( MEDIA_FNAME_ );
    invalidate_all_records_mps();

    REQUIRE( are_all_records_mps_valid() == false );

    // Load the records
    recordServer_.open();
    Persistence::Record::ServerStatus serverStatus = Persistence::Record::ServerStatus::eUNKNOWN;
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    Cpl::System::Api::sleep( 500 );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );

    // Update records...
    record1_.m_mp1.write( "BOB-Rec1Mp1" );
    record1_.m_mp2.write( "BOB-Rec1Mp2" );
    record3_.m_mp3.write( "BOB-Rec3Mp3" );
    Cpl::System::Api::sleep( 1000 );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );

    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    //
    // Open Existing file - with updated values
    //

    // Load the records
    invalidate_all_records_mps();
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );
   
    // Default Record 3
    mp_recordDefaultActionRequest_.write( "Record3" );
    Cpl::System::Api::sleep( 500 );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );
   
    // Default ALL Records
    mp_recordDefaultActionRequest_.write( "*" );
    Cpl::System::Api::sleep( 500 );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );
    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    // Load the records
    recordServer_.open();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eRUNNING );
    Cpl::System::Api::sleep( 500 );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );
    recordServer_.close();
    REQUIRE( ModelPoint::IS_VALID( mp_recordServerStatus_.read( serverStatus ) ) );
    REQUIRE( serverStatus == +Persistence::Record::ServerStatus::eCLOSED );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


TEST_CASE( "recordserver-last", "[recordserver-last]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-LAST test" );

    // Shutdown threads
    chunkMailbox_.pleaseStop();
    recordServerMailbox_.pleaseStop();
    Cpl::System::Api::sleep( 200 ); // allow time for threads to stop
    REQUIRE( t1_->isRunning() == false );
    REQUIRE( t2_->isRunning() == false );
    Cpl::System::Thread::destroy( *t1_ );
    Cpl::System::Thread::destroy( *t2_ );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
