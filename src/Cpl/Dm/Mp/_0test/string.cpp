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
#include "Cpl/Math/real.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/String.h"
#include "common.h"
#include <string.h>


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
#define APPLE_MAX_SIZE  10
static StaticInfo      info_mp_apple_( "APPLE" );
static Mp::String      mp_apple_( modelDb_, info_mp_apple_, APPLE_MAX_SIZE );

#define ORANGE_MAX_SIZE 15
static StaticInfo      info_mp_orange_( "ORANGE" );
static Mp::String      mp_orange_( modelDb_, info_mp_orange_, ORANGE_MAX_SIZE, "bobs yours uncle (should get truncated)" );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "string-readwrite", "[string-readwrite]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-READWRITE test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Read1
    char             myDataMemory[5];
    Mp::String::Data myData = { myDataMemory, 0, sizeof( myDataMemory ) - 1 };
    uint16_t         seqNum;
    int8_t           valid = mp_orange_.read( myData );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( strcmp( myData.stringPtr, "bobs" ) == 0 );

    // Read2
    Cpl::Text::FString<32> value;
    valid = mp_orange_.read( value, &seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == "bobs yours uncl" );
    valid = mp_apple_.read( value, &seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Write1
    uint16_t seqNum2 = mp_apple_.write( "-10.1234F" );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == "-10.1234F" );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Write2
    seqNum = mp_apple_.write( "-10.1234F", 3 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == "-10" );
    REQUIRE( seqNum == seqNum2 + 1 );

    // Write3
    Mp::String::Data myData2 = { (char*)"bob was here today", 18, 18 };
    seqNum2 = mp_apple_.write( myData2 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == "bob was he" );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read-Modify-Write with Lock (with too long of string)
    RmwString callbackClient;
    callbackClient.m_callbackCount  = 0;
    callbackClient.m_valueToWrite   = "Hello bob.!";
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eLOCK );
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    bool locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( value == "Hello bob." );
    REQUIRE( callbackClient.m_callbackCount == 1 );

    // Read-Modify-Write with Lock 
    mp_apple_.removeLock();
    callbackClient.m_callbackCount  = 0;
    callbackClient.m_valueToWrite   = "Hello bob!";
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eLOCK );
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( value == "Hello bob!" );
    REQUIRE( callbackClient.m_callbackCount == 1 );

    // Invalidate with Unlock
    mp_apple_.setInvalidState( 112, ModelPoint::eUNLOCK );
    REQUIRE( mp_apple_.isNotValid() == true );
    valid = mp_apple_.getValidState();
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );
    REQUIRE( valid == 112 );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "string-get", "[string-get]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-GET test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Gets...
    const char* name = mp_apple_.getName();
    REQUIRE( strcmp( name, "APPLE" ) == 0 );
    name = mp_orange_.getName();
    REQUIRE( strcmp( name, "ORANGE" ) == 0 );

    size_t s = mp_apple_.getSize();
    REQUIRE( s == APPLE_MAX_SIZE );
    s = mp_orange_.getSize();
    REQUIRE( s == ORANGE_MAX_SIZE );

    s = mp_apple_.getExternalSize();
    REQUIRE( s == APPLE_MAX_SIZE + sizeof( int8_t ) );
    s = mp_orange_.getExternalSize();
    REQUIRE( s == ORANGE_MAX_SIZE + sizeof( int8_t ) );

    const char* mpType = mp_apple_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s])", mpType) );
    REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::String" ) == 0 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
#define STREAM_BUFFER_SIZE  100

TEST_CASE( "string-export", "[string-export]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-EXPORT test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //  Export/Import Buffer
    uint8_t streamBuffer[STREAM_BUFFER_SIZE];
    REQUIRE( mp_apple_.getExternalSize() <= STREAM_BUFFER_SIZE );


    // Export invalid...
    REQUIRE( mp_apple_.isNotValid() == true );
    uint16_t seqNum  = mp_apple_.getSequenceNumber();
    uint16_t seqNum2 = mp_apple_.getSequenceNumber();
    size_t b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // Update the MP
    seqNum = mp_apple_.write( "-42.14159F" );
    REQUIRE( seqNum == seqNum2 + 1 );
    Cpl::Text::FString<32> value;
    int8_t                 valid;
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == "-42.14159F" );

    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read import value/state
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Update the MP
    seqNum = mp_apple_.write( "13.99F" );
    REQUIRE( seqNum == seqNum2 + 1 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == "13.99F" );

    // Export...
    REQUIRE( mp_apple_.isNotValid() == false );
    b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // Set a new value AND invalidate the MP
    mp_apple_.write( "bob's here" );
    seqNum = mp_apple_.setInvalid();
    REQUIRE( seqNum == seqNum2 + 2 );
    REQUIRE( mp_apple_.isNotValid() == true );

    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read import value/state
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == "13.99F" );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
#define MAX_STR_LENG    20

TEST_CASE( "string-tostring", "[string-tostring]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-TOSTRING test" );
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
    mp_apple_.write( "127.14F", ModelPoint::eUNLOCK );
    mp_apple_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum + 1 );
    REQUIRE( string == "\"127.14F\"" );

    // Value + Lock
    mp_apple_.applyLock();
    mp_apple_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum + 1 );
    REQUIRE( string == "!\"127.14F\"" );
    mp_apple_.removeLock();

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "string-fromstring", "[string-fromstring]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-FROMSTRING test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Start with MP in the invalid state
    Cpl::Text::FString<MAX_STR_LENG> string;
    Cpl::Text::DString               errorMsg( "noerror", 1024 );
    mp_apple_.removeLock();
    mp_orange_.removeLock();
    mp_orange_.setInvalid();
    uint16_t seqNum = mp_apple_.setInvalid();
    uint16_t seqNum2;

    // Write value
    const char* nextChar = mp_apple_.fromString( "12.34", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum + 1 );
    Cpl::Text::FString<32> value;
    int8_t                 valid;
    valid = mp_apple_.read( value, &seqNum );
    REQUIRE( seqNum == seqNum2 );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == "12.34" );
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

    // Write with Lock
    nextChar = mp_apple_.fromString( "!111.3", 0, 0, &seqNum );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 + 1 == seqNum );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( mp_apple_.isLocked() == true );

    // Write while locked
    errorMsg = "noerror";
    nextChar = mp_apple_.fromString( "112.01", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( mp_apple_.isLocked() == true );
    valid = mp_apple_.read( value, &seqNum );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == "111.3" );
    REQUIRE( mp_apple_.isLocked() == true );
    REQUIRE( errorMsg == "noerror" );

    // Write with unlock
    REQUIRE( mp_orange_.isLocked() == true );
    nextChar = mp_orange_.fromString( "^3.14159", 0, &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    valid = mp_orange_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == "3.14159" );


    // Just lock
    nextChar = mp_orange_.fromString( "!", 0, &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == true );
    valid = mp_orange_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == "3.14159" );

    // Test termination characters
    nextChar = mp_orange_.fromString( "^\"4.321\",", ": .,;", &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == ',' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    valid = mp_orange_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == "4.321" );

    // Test "Text Strings"
    nextChar = mp_orange_.fromString( "^\"4`\".321\",", ": .,;", &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == ',' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    valid = mp_orange_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == "4\".321" );

    // Test "Text Strings"
    nextChar = mp_orange_.fromString( "^\"4`\".321 and,more text\",", ": .,;", &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == ',' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    valid = mp_orange_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == "4\".321 and,more" );

    // Test ERROR in a "Text Strings"
    errorMsg = "noerror";
    nextChar = mp_orange_.fromString( "\"0x1234", 0, &errorMsg );
    REQUIRE( nextChar == 0 );
    valid = mp_orange_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == "4\".321 and,more" );
    REQUIRE( errorMsg != "noerror" );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("fromString FAILED: errorMsg=[%s])", errorMsg.getString()) );


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////

static Cpl::Dm::MailboxServer     t1Mbox_;

TEST_CASE( "string-observer", "[string-observer]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-OBSERVER test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
    ViewerString viewer1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );

    // Open, write a value, wait for Viewer to see the change, then close
    mp_apple_.removeLock();
    viewer1.open();
    uint16_t seqNum = mp_apple_.write( "33" );
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

