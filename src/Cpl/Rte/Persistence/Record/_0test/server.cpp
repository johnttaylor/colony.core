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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Checksum/Crc32EthernetFast.h"
#include "Cpl/Rte/ModelDatabase.h"
#include "Cpl/Rte/Persistence/Record/Server.h"
#include "Cpl/Rte/Persistence/Chunk/FileSystem.h"
#include "Cpl/Rte/Persistence/Chunk/Server.h"
#include "Cpl/Io/File/Api.h"

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
static Cpl::Rte::StaticInfo                     info_mp_recordServerStatus_( "RecordServerStatus" );
static Persistence::Record::MpServerStatus      mp_recordServerStatus_( modelDb_, info_mp_recordServerStatus_ );
static Cpl::Rte::StaticInfo                     info_mp_recordDefaultActionRequest_( "RecordServerDefault" );
static Mp::String                               mp_recordDefaultActionRequest_( modelDb_, info_mp_recordDefaultActionRequest_, MP_NAME_MAX_LEN );


static Cpl::Rte::StaticInfo                     info_mp_rec1Mp1_( "Rec1Mp1" );
static Cpl::Rte::StaticInfo                     info_mp_rec1Mp2_( "Rec1Mp2" );
static Cpl::Rte::StaticInfo                     info_mp_rec1Mp3_( "Rec1Mp3" );
static Mp::String                               mp_rec1Mp1_( modelDb_, info_mp_rec1Mp1_, MP_MAX_LEN );
static Mp::String                               mp_rec1Mp2_( modelDb_, info_mp_rec1Mp2_, MP_MAX_LEN );
static Mp::String                               mp_rec1Mp3_( modelDb_, info_mp_rec1Mp3_, MP_MAX_LEN );

static Cpl::Rte::StaticInfo                     info_mp_rec2Mp1_( "Rec2Mp1" );
static Cpl::Rte::StaticInfo                     info_mp_rec2Mp2_( "Rec2Mp2" );
static Cpl::Rte::StaticInfo                     info_mp_rec2Mp3_( "Rec2Mp3" );
static Mp::String                               mp_rec2Mp1_( modelDb_, info_mp_rec2Mp1_, MP_MAX_LEN );
static Mp::String                               mp_rec2Mp2_( modelDb_, info_mp_rec2Mp2_, MP_MAX_LEN );
static Mp::String                               mp_rec2Mp3_( modelDb_, info_mp_rec2Mp3_, MP_MAX_LEN );

static Cpl::Rte::StaticInfo                     info_mp_rec3Mp1_( "Rec3Mp1" );
static Cpl::Rte::StaticInfo                     info_mp_rec3Mp2_( "Rec3Mp2" );
static Cpl::Rte::StaticInfo                     info_mp_rec3Mp3_( "Rec3Mp3" );
static Mp::String                               mp_rec3Mp1_( modelDb_, info_mp_rec3Mp1_, MP_MAX_LEN );
static Mp::String                               mp_rec3Mp2_( modelDb_, info_mp_rec3Mp2_, MP_MAX_LEN );
static Mp::String                               mp_rec3Mp3_( modelDb_, info_mp_rec3Mp3_, MP_MAX_LEN );

static Cpl::Checksum::Crc32EthernetFast         chunkCrc_;
static Persistence::Chunk::FileSystem           mediaFile( MEDIA_FNAME_ );
static Persistence::Chunk::Server               chunkServer_( mediaFile, chunkCrc_, SIGNATURE_ );
static Cpl::Itc::MailboxServer                  chunkMailbox_;
static Persistence::Chunk::Request::SAP         chunkSap_( chunkServer_, chunkMailbox_ );

static uint8_t                                          recordLayerbuffer_[MAX_BUF_SIZE_];
static Cpl::Container::Map<Persistence::Record::Api_>   recordList_;
static Cpl::Rte::MailboxServer                          recordServerMailbox_;
static Persistence::Record::Server                      recordServer_( recordLayerbuffer_,
                                                                       sizeof( recordLayerbuffer_ ),
                                                                       chunkSap_,
                                                                       recordServerMailbox_,
                                                                       recordList_,
                                                                       mp_recordServerStatus_,
                                                                       mp_recordDefaultActionRequest_ );

static MyRecord                                 record1_( recordList_, 0, "Record1", recordServerMailbox_, mp_rec1Mp1_, mp_rec1Mp2_, mp_rec1Mp3_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" );
static MyRecord                                 record2_( recordList_, 0, "Record2", recordServerMailbox_, mp_rec2Mp1_, mp_rec2Mp2_, mp_rec2Mp3_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" );
static MyRecord                                 record3_( recordList_, 0, "Record3", recordServerMailbox_, mp_rec3Mp1_, mp_rec3Mp2_, mp_rec3Mp3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" );

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
    int8_t                         mpState;
    Cpl::Text::FString<MP_MAX_LEN> value;
    record.m_mp1.read( value, mpState ); if ( Cpl::Rte::ModelPoint::IS_VALID( mpState ) == false || value != mp1Value ) return false;
    record.m_mp2.read( value, mpState ); if ( Cpl::Rte::ModelPoint::IS_VALID( mpState ) == false || value != mp2Value ) return false;
    record.m_mp3.read( value, mpState ); if ( Cpl::Rte::ModelPoint::IS_VALID( mpState ) == false || value != mp3Value ) return false;
    return true;
}

static Cpl::System::Thread* t1_;
static Cpl::System::Thread* t2_;

TEST_CASE( "recordserver-open", "[recordserver-open]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-OPEN test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Ensure starting conditions
    Cpl::Io::File::Api::remove( MEDIA_FNAME_ );
    invalidate_all_records_mps();

    t1_ = Cpl::System::Thread::create( chunkMailbox_, "Chunk" );
    t2_ = Cpl::System::Thread::create( recordServerMailbox_, "Record" );

    REQUIRE( are_all_records_mps_valid() == false );

    // Load the records
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );
    recordServer_.close();

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "recordserver-write", "[recordserver-write]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-WRITE test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Ensure starting conditions
    invalidate_all_records_mps();

    REQUIRE( are_all_records_mps_valid() == false );

    // Load the records
    recordServer_.open();
    Cpl::System::Api::sleep( 500 );
    REQUIRE( are_all_records_mps_valid() == true );
    REQUIRE( are_record_values_match( record1_, "Rec1Mp1Default", "Rec1Mp2Default", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "Rec3Mp3Default" ) );

    // Update records...
    record1_.m_mp1.write( "BOB-Rec1Mp1" );
    record1_.m_mp2.write( "BOB-Rec1Mp2" );
    record3_.m_mp3.write( "BOB-Rec3Mp3" );
    Cpl::System::Api::sleep( 100 );
    REQUIRE( are_record_values_match( record1_, "BOB-Rec1Mp1", "BOB-Rec1Mp2", "Rec1Mp3Default" ) );
    REQUIRE( are_record_values_match( record2_, "Rec2Mp1Default", "Rec2Mp2Default", "Rec2Mp3Default" ) );
    REQUIRE( are_record_values_match( record3_, "Rec3Mp1Default", "Rec3Mp2Default", "BOB-Rec3Mp3" ) );

    recordServer_.close();
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
