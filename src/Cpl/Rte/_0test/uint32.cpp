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
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Rte/ModelDatabase.h"
#include "Cpl/Rte/Mp/Uint32.h"
#include "common.h"
#include <string.h>


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static StaticInfo       info_mp_apple_( "APPLE" );
static Mp::Uint32       mp_apple_( modelDb_, info_mp_apple_ );

static StaticInfo       info_mp_orange_( "ORANGE" );
static Mp::Uint32       mp_orange_( modelDb_, info_mp_orange_, 32, false );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "uint32-get", "[uint32-get]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "UINT32-GET test" );
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
    REQUIRE( strcmp( mpType, "Cpl::Rte::Mp::Uint32-dec" ) == 0 );

    mpType = mp_orange_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s])", mpType) );
    REQUIRE( strcmp( mpType, "Cpl::Rte::Mp::Uint32-hex" ) == 0 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
#define STREAM_BUFFER_SIZE  100

TEST_CASE( "uint32-export", "[uint32-export]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "UINT32-EXPORT test" );
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
    seqNum = mp_apple_.write( 42 );
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
    seqNum = mp_apple_.write( 13 );
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

    // set a new value AND invalidate the MP
    mp_apple_.write( 666 );
    seqNum = mp_apple_.setInvalid();
    REQUIRE( seqNum == seqNum2 + 2 );
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
#define MAX_STR_LENG    20

TEST_CASE( "uint32-tostring", "[uint32-tostring]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "UINT32-TOSTRING test" );
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
    mp_apple_.write( 127, ModelPoint::eUNLOCK );
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

    // Hex Value 
    seqNum = mp_orange_.getSequenceNumber();
    mp_orange_.write( 0xBEEF );
    mp_orange_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum + 1 );
    REQUIRE( string == "0xBEEF" );

    // Hex Value + Lock
    mp_orange_.applyLock();
    mp_orange_.toString( string, false, &seqNum2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("toString: [%s])", string.getString()) );
    REQUIRE( seqNum2 == seqNum + 1 );
    REQUIRE( string == "!0xBEEF" );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "uint32-fromstring", "[uint32-fromstring]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "UINT32-FROMSTRING test" );
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
    const char* nextChar = mp_apple_.fromString( "1234", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum + 1 );
    uint32_t value;
    int8_t   valid;
    seqNum = mp_apple_.read( value, valid );
    REQUIRE( seqNum == seqNum2 );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 1234 );
    REQUIRE( errorMsg == "noerror" );

    // Write Hex value- Fail case
    nextChar = mp_apple_.fromString( "0x1234", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar == 0 );
    seqNum2 = mp_apple_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( seqNum == seqNum2 );
    REQUIRE( value == 1234 );
    REQUIRE( errorMsg != "noerror" );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("fromString FAILED: errorMsg=[%s])", errorMsg.getString()) );

    // Write Hex value- Pass case
    errorMsg = "noerror";
    nextChar = mp_orange_.fromString( "0x1234", 0, &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 0x1234 );
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
    nextChar = mp_apple_.fromString( "!111", 0, 0, &seqNum );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 + 1 == seqNum );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( mp_apple_.isLocked() == true );

    // Write while locked
    errorMsg = "noerror";
    nextChar = mp_apple_.fromString( "112", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( mp_apple_.isLocked() == true );
    seqNum = mp_apple_.read( value, valid );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 111 );
    REQUIRE( mp_apple_.isLocked() == true );
    REQUIRE( errorMsg == "noerror" );

    // Write with unlock
    REQUIRE( mp_orange_.isLocked() == true );
    nextChar = mp_orange_.fromString( "^0xDEAD", 0, &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 0xDEAD );

    // Just lock
    nextChar = mp_orange_.fromString( "!", 0, &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == true );
    mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 0xDEAD );

    // Test termination characters
    nextChar = mp_orange_.fromString( "^0x4321,", ": ,;", &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == ',' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    mp_orange_.read( value, valid );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value == 0x4321);

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}