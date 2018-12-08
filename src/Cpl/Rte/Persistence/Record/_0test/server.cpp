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

#include "common.h"
#include <string.h>
#include <stdint.h>

/// This method is used as part of 'forcing' this object to being actually 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case)
void link_server( void ) {}




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
static Cpl::Container::Map<Persistence::Record::Api_>&  recordList_;
static Cpl::Rte::MailboxServer                          recordServerMailbox_;
static Persistence::Record::Server                      recordServer_( recordLayerbuffer_,
                                                                       sizeof( recordLayerBuffer_ ),
                                                                       chunkSap_
                                                                       recordServerMailbox_,
                                                                       recordList_,
                                                                       mp_recordServerStatus_,
                                                                       mp_recordDefaultActionRequest_ );

static MyRecord                                 record1_( recordList_, 0, recordServerMailbox_, mp_rec1Mp1_, mp_rec1Mp2_, mp_rec1Mp3_ );
static MyRecord                                 record2_( recordList_, 0, recordServerMailbox_, mp_rec2Mp1_, mp_rec2Mp2_, mp_rec2Mp3_ );
static MyRecord                                 record2_( recordList_, 0, recordServerMailbox_, mp_rec3Mp1_, mp_rec3Mp2_, mp_rec3Mp3_ );

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

TEST_CASE( "recordserver-open", "[recordserver-open]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-OPEN test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    invalidate_all_records_mps();

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( chunkMailbox_, "Chunk" );
    Cpl::System::Thread* t2 = Cpl::System::Thread::create( recordServerMailbox_, "Record" );

    REQUIRE( are_all_records_mps_valid() == false );

    // Load the records
    recordServer_.open();
    REQUIRE( are_all_records_mps_valid() == true );
    recordServer_.close();


    // Shutdown threads
    chunkMailbox_.pleaseStop();
    recordServerMailbox_.pleaseStop();
    Cpl::System::Api::sleep( 200 ); // allow time for threads to stop
    REQUIRE( t1->isRunning() == false );
    REQUIRE( t2->isRunning() == false );
    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Thread::destroy( *t2 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
