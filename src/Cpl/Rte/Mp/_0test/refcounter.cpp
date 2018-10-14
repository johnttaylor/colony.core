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
#include "Cpl/Rte/ModelDatabase.h"
#include "Cpl/Rte/Mp/RefCounter.h"
#include "common.h"
#include <string.h>

/// This method is used as part of 'forcing' this object to being actually 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case)
void link_refcounter( void ) {}




////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static StaticInfo      info_mp_apple_( "APPLE" );
static Mp::RefCounter  mp_apple_( modelDb_, info_mp_apple_ );

static StaticInfo      info_mp_orange_( "ORANGE" );
static Mp::RefCounter  mp_orange_( modelDb_, info_mp_orange_, ModelPoint::MODEL_POINT_STATE_VALID, 64 );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "refcounter-readwrite", "[refcounter-readwrite]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-READWRITE test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Read
    uint32_t value;
    int8_t   valid;
    uint16_t seqNum = mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 64 );
    seqNum = mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Write (reset). -->Count = 0
    uint16_t seqNum2 = mp_apple_.reset();
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 0 );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Write (decrement)  -->Count = 0
    seqNum = mp_apple_.decrement();
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 0 );
    REQUIRE( seqNum == seqNum2 );

    // Write (increment)  -->Count = 1
    seqNum2 = mp_apple_.increment();
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 1 );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Write (decrement) -->Count = 0
    seqNum = mp_apple_.decrement();
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 0 );
    REQUIRE( seqNum == seqNum2 + 1 );

    // Write (decrement) -->Count = 0
    seqNum2 = mp_apple_.decrement();
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 0 );
    REQUIRE( seqNum == seqNum2 );       // No a change  transition -->so no change in the sequence number

    // Write (increment)  -->Count = max value
    seqNum2 = mp_apple_.increment( (uint32_t) -1 );
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == (uint32_t) -1 );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Write (increment) -->Count = max value
    seqNum = mp_apple_.increment();
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == (uint32_t) -1 );
    REQUIRE( seqNum == seqNum2 );

    // Write with lock (decrement) -->Count = 2
    seqNum2 = mp_apple_.decrement( (uint32_t) -3, ModelPoint::eLOCK );
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 2 );
    REQUIRE( seqNum == seqNum2 );   // No a change  transition -->so no change in the sequence number

    // Write (increment) -->Count = 2
    seqNum = mp_apple_.increment();
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 2 );
    REQUIRE( seqNum == seqNum2 );

    // Invalidate with Unlock
    mp_apple_.setInvalidState( 112, ModelPoint::eUNLOCK );
    REQUIRE( mp_apple_.isNotValid() == true );
    valid = mp_apple_.getValidState();
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );
    REQUIRE( valid == 112 );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "refcounter-get", "[refcounter-get]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-GET test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Gets...
    const char* name = mp_apple_.getName();
    REQUIRE( strcmp( name, "APPLE" ) == 0 );
    name = mp_orange_.getName();
    REQUIRE( strcmp( name, "ORANGE" ) == 0 );

    size_t s = mp_apple_.getSize();
    REQUIRE( s == sizeof( uint32_t ) );
    s = mp_orange_.getSize();
    REQUIRE( s == sizeof( uint32_t ) );

    s = mp_apple_.getExternalSize();
    REQUIRE( s == sizeof( uint32_t ) + sizeof( int8_t ) );
    s = mp_orange_.getExternalSize();
    REQUIRE( s == sizeof( uint32_t ) + sizeof( int8_t ) );

    const char* mpType = mp_apple_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s])", mpType) );
    REQUIRE( strcmp( mpType, "REFCOUNTER" ) == 0 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
#define STREAM_BUFFER_SIZE  100

TEST_CASE( "refcounter-export", "[refcounter-export]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-EXPORT test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //  Export/Import Buffer
    uint8_t streamBuffer[STREAM_BUFFER_SIZE];
    REQUIRE( mp_apple_.getExternalSize() <= STREAM_BUFFER_SIZE );


    // Export...
    REQUIRE( mp_apple_.isNotValid() == true );
    uint16_t seqNum  = mp_apple_.getSequenceNumber();
    uint16_t seqNum2 = mp_apple_.getSequenceNumber();
    size_t b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // Update the MP
    seqNum = mp_apple_.reset( 42 );
    REQUIRE( seqNum == seqNum2 + 1 );
    uint32_t value;
    int8_t   valid;
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == 42 );

    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read import value/state
    mp_apple_.read( value, valid );
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Update the MP
    seqNum = mp_apple_.reset( 13 );
    REQUIRE( seqNum == seqNum2 + 1 );
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == 13 );

    // Export...
    REQUIRE( mp_apple_.isNotValid() == false );
    b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // Invalidate the MP
    seqNum = mp_apple_.setInvalid();
    REQUIRE( seqNum == seqNum2 + 1 );
    REQUIRE( mp_apple_.isNotValid() == true );

    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read import value/state
    mp_apple_.read( value, valid );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 13 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
///////////////////////////////////////////////////////////////////////////////
#define MAX_STR_LENG    128

TEST_CASE( "refcounter-tostring", "[refcounter-tostring]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-TOSTRING test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Invalid (Default value)
    Cpl::Text::FString<MAX_STR_LENG> string;
    uint16_t seqNum = mp_apple_.setInvalid();
    uint16_t seqNum2;
    mp_apple_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( string == "?" );

    // Invalid (Default value) + Locked
    mp_apple_.applyLock();
    mp_apple_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( string == "!?" );

    // Invalid (custom value)
    mp_apple_.removeLock();
    seqNum = mp_apple_.setInvalidState( 100 );
    mp_apple_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( string == "?100" );

    // Invalid (custom value) + Locked
    mp_apple_.applyLock();
    mp_apple_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( string == "!?100" );

    // Value 
    mp_apple_.reset( 127, ModelPoint::eUNLOCK );
    mp_apple_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum + 1 );
    REQUIRE( string == "127" );

    // Value + Lock
    mp_apple_.applyLock();
    mp_apple_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum + 1 );
    REQUIRE( string == "!127" );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "refcounter-fromstring", "[refcounter-fromstring]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-FROMSTRING test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Start with MP in the invalid state
    Cpl::Text::FString<MAX_STR_LENG> string;
    Cpl::Text::DString               errorMsg( "noerror", 1024 );
    mp_apple_.removeLock();
    mp_orange_.removeLock();
    mp_orange_.setInvalid();
    uint16_t seqNum = mp_apple_.reset(3);
    uint16_t seqNum2;

    // Increment the value
    const char* nextChar = mp_apple_.fromString( "+2", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum );       // Note: No 'change' because transition from 3 -> 5
    uint32_t value;
    int8_t   valid;
    seqNum = mp_apple_.read( value, valid );
    REQUIRE( seqNum == seqNum2 );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 5 );
    REQUIRE( errorMsg == "noerror" );

    // Decrement the value
    nextChar = mp_apple_.fromString( "-1", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum );       // Note: No 'change' because transition from 5 -> 4
    seqNum = mp_apple_.read( value, valid );
    REQUIRE( seqNum == seqNum2 );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 4 );
    REQUIRE( errorMsg == "noerror" );

    // Set Invalid
    nextChar = mp_apple_.fromString( "?", 0, 0, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum + 1 );
    REQUIRE( mp_apple_.isNotValid() == true );

    // Set Invalid with lock
    REQUIRE( mp_apple_.isLocked() == false );
    nextChar = mp_orange_.fromString( "!?" );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == true );
    REQUIRE( mp_orange_.isLocked() == true );

    // Increment with Lock
    nextChar = mp_apple_.fromString( "!+6", 0, 0, &seqNum );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 + 1 == seqNum );       
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( mp_apple_.isLocked() == true );
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 6 );      // NOTE: Transition from invalid to valid which means the counter:= 0 + 6

    // Decrement while locked
    errorMsg = "noerror";
    nextChar = mp_apple_.fromString( "-5", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( mp_apple_.isLocked() == true );
    seqNum = mp_apple_.read( value, valid );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 6 );
    REQUIRE( mp_apple_.isLocked() == true );
    REQUIRE( errorMsg == "noerror" );

    // Reset/Set with unlock
    REQUIRE( mp_orange_.isLocked() == true );
    nextChar = mp_orange_.fromString( "^4", 0, &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 4 );

    // Just lock
    nextChar = mp_orange_.fromString( "!", 0, &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == true );
    mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 4 );

    // Test termination characters AND 'reset'
    nextChar = mp_orange_.fromString( "^11,", ": ,;", &errorMsg, &seqNum );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == ',' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 11 );

    // Write Fail case
    errorMsg = "noerror";
    nextChar = mp_orange_.fromString( "+", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar == 0 );
    seqNum2 = mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( seqNum == seqNum2 );
    REQUIRE( value == 11 );
    REQUIRE( errorMsg != "noerror" );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("fromString FAILED: errorMsg=[%s]", errorMsg.getString()) );

    // Write Fail case2
    errorMsg = "noerror";
    nextChar = mp_orange_.fromString( "+2.", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar == 0 );
    seqNum2 = mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( seqNum == seqNum2 );
    REQUIRE( value == 11 );
    REQUIRE( errorMsg != "noerror" );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("fromString FAILED: errorMsg=[%s]", errorMsg.getString()) );


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
static Cpl::Rte::MailboxServer     t1Mbox_;

#define NUM_CHANGE_NOTIFICATIONS    5 // Register, Invalid ->1, 1->0, 0->Invalid
#define DELAY_TO_NOT_MISS_EDGES     100

TEST_CASE( "refcounter-observer", "[refcounter-observer]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-OBSERVER test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
    ViewerRefCounter viewer1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_, NUM_CHANGE_NOTIFICATIONS );

    // Open, write a value, wait for Viewer to see the change, then close
    mp_apple_.removeLock();
    mp_apple_.setInvalid();
    viewer1.open();

    // Invalid ->1..4
    uint16_t seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    // 4..1 ->0
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    // 0 -> 0 (no change notification)
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    // 0 -> 1
    seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    // 1 -> Invalid
    seqNum = mp_apple_.setInvalid();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    Cpl::System::Thread::wait();
    viewer1.close();
    REQUIRE( viewer1.m_lastSeqNumber == seqNum );

    // Shutdown threads
    t1Mbox_.pleaseStop();
    Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
    REQUIRE( t1->isRunning() == false );
    Cpl::System::Thread::destroy( *t1 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
