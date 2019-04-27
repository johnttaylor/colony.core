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
#include "Cpl/Dm/ModelDatabase.h"
#include "common.h"
#include <string.h>


#define STRCMP(s1,s2)  (strcmp(s1,s2)==0)

////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Cpl::Dm::StaticInfo                  info_mp_apple_( "APPLE" );
static Persistence::Record::MpServerStatus  mp_apple_( modelDb_, info_mp_apple_ );

static Cpl::Dm::StaticInfo                  info_mp_orange_( "ORANGE" );
static Persistence::Record::MpServerStatus  mp_orange_( modelDb_, info_mp_orange_, Persistence::Record::ServerStatus::eRUNNING_MINOR_UPGRADE );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "serverstatus-readwrite", "[serverstatus-readwrite]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-READWRITE test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Read
    Persistence::Record::ServerStatus value = Persistence::Record::ServerStatus::eRUNNING;
    uint16_t seqNum;
    int8_t valid = mp_orange_.read( value, &seqNum );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("read:orange:= [%s])", value._to_string()) );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == +Persistence::Record::ServerStatus::eRUNNING_MINOR_UPGRADE );
    valid = mp_apple_.read( value, &seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Write
    uint16_t seqNum2 = mp_apple_.write( Persistence::Record::ServerStatus::eOPENING );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("write:apple:= [%s])", value._to_string()) );
    REQUIRE( value == +Persistence::Record::ServerStatus::eOPENING );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read-Modify-Write with Lock
    RmwServerStatus callbackClient;
    callbackClient.m_callbackCount  = 0;
    callbackClient.m_incValue       = 1;
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eLOCK );
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    bool locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( value == +Persistence::Record::ServerStatus::eRUNNING );
    REQUIRE( callbackClient.m_callbackCount == 1 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("rmw:apple:= [%s])", value._to_string()) );

    // Invalidate with Unlock
    mp_apple_.setInvalidState( 112, ModelPoint::eUNLOCK );
    REQUIRE( mp_apple_.isNotValid() == true );
    valid = mp_apple_.getValidState();
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );
    REQUIRE( valid == 112 );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "serverstatus-get", "[serverstatus-get]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-GET test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Gets...
    const char* name = mp_apple_.getName();
    REQUIRE( strcmp( name, "APPLE" ) == 0 );
    name = mp_orange_.getName();
    REQUIRE( strcmp( name, "ORANGE" ) == 0 );

    size_t s = mp_apple_.getSize();
    REQUIRE( s == sizeof( int ) );
    s = mp_orange_.getSize();
    REQUIRE( s == sizeof( int ) );

    s = mp_apple_.getExternalSize();
    REQUIRE( s == sizeof( int ) + sizeof( int8_t ) );
    s = mp_orange_.getExternalSize();
    REQUIRE( s == sizeof( int ) + sizeof( int8_t ) );

    const char* mpType = mp_apple_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s])", mpType) );
    REQUIRE( strcmp( mpType, "Cpl::Dm::Persistence::Record::ServerStatus" ) == 0 );

    mpType = mp_orange_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s])", mpType) );
    REQUIRE( strcmp( mpType, "Cpl::Dm::Persistence::Record::ServerStatus" ) == 0 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
#define STREAM_BUFFER_SIZE  100

TEST_CASE( "serverstatus-export", "[serverstatus-export]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-EXPORT test" );
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
    seqNum = mp_apple_.write( Persistence::Record::ServerStatus::eUNKNOWN );
    REQUIRE( seqNum == seqNum2 + 1 );
    Persistence::Record::ServerStatus   value = Persistence::Record::ServerStatus::eRUNNING;
    int8_t   valid= mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == +Persistence::Record::ServerStatus::eUNKNOWN );

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
    seqNum = mp_apple_.write( Persistence::Record::ServerStatus::eOPENING );
    REQUIRE( seqNum == seqNum2 + 1 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == +Persistence::Record::ServerStatus::eOPENING );

    // Export...
    REQUIRE( mp_apple_.isNotValid() == false );
    b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // Set and new value AND invalidate the MP
    mp_apple_.write( Persistence::Record::ServerStatus::eRUNNING );
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
    REQUIRE( value == +Persistence::Record::ServerStatus::eOPENING );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
#define MAX_STR_LENG    1024

TEST_CASE( "serverstatus-toJSON" )
{
	CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-TOJSON test" );
	Cpl::System::Shutdown_TS::clearAndUseCounter();
	char string[MAX_STR_LENG + 1];
	bool truncated;


	SECTION( "Invalid" )
	{
		// Invalid (Default value)
		uint16_t seqnum = mp_apple_.setInvalid();
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );
		REQUIRE( truncated == false );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( STRCMP( doc["name"], "APPLE" ) );
		REQUIRE( STRCMP( doc["type"], mp_apple_.getTypeAsText() ) );
		REQUIRE( doc["seqnum"] == seqnum );
		REQUIRE( doc["locked"] == false );
		REQUIRE( doc["invalid"] > 0 );
	}

	SECTION( "Invalid + Locked" )
	{
		mp_apple_.applyLock();
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["locked"] == true );
		REQUIRE( doc["invalid"] > 0 );
	}

	SECTION( "Invalid - custom value" )
	{
		mp_apple_.removeLock();
		uint16_t seqnum = mp_apple_.setInvalidState( 100 );
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["seqnum"] == seqnum );
		REQUIRE( doc["invalid"] == 100 );
		REQUIRE( doc["locked"] == false );
	}

	SECTION( "Invalid - custom value + locked" )
	{
		// Invalid (custom value) + Locked
		mp_apple_.applyLock();
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["invalid"] == 100 );
		REQUIRE( doc["locked"] == true );
	}

	SECTION( "Value" )
	{
		uint16_t seqnum = mp_apple_.write( Persistence::Record::ServerStatus::eRUNNING, ModelPoint::eUNLOCK );
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["seqnum"] == seqnum );
		REQUIRE( doc["locked"] == false );
		REQUIRE( doc["invalid"] == 0 );
		REQUIRE( STRCMP( doc["val"], "eRUNNING" ) );
	}

	SECTION( "Value + Lock" )
	{
		mp_apple_.applyLock();
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["locked"] == true );
		REQUIRE( doc["invalid"] == 0 );
		REQUIRE( STRCMP( doc["val"], "eRUNNING" ) );
	}

	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "enum-fromJSON" )
{
	CPL_SYSTEM_TRACE_SCOPE( SECT_, "ENUM-FROMJSON test" );
	Cpl::System::Shutdown_TS::clearAndUseCounter();

	// Start with MP in the invalid state
	Cpl::Text::FString<MAX_STR_LENG> string;
	Cpl::Text::DString               errorMsg( "noerror", 1024 );
	mp_apple_.removeLock();
	mp_orange_.removeLock();
	mp_orange_.setInvalid();
	uint16_t seqNum = mp_apple_.setInvalid();
	uint16_t seqNum2;
	ModelPoint* mp;

	SECTION( "Write value" )
	{
		const char* json = "{name:\"APPLE\", val:\"eNO_STORAGE_MEDIA_ERR\"}";
		bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( seqNum2 == seqNum + 1 );
		Persistence::Record::ServerStatus value = Persistence::Record::ServerStatus::eUNKNOWN;
		int8_t   valid = mp_apple_.read( value, &seqNum );
		REQUIRE( seqNum == seqNum2 );
		REQUIRE( ModelPoint::IS_VALID( valid ) );
		REQUIRE( value == +Persistence::Record::ServerStatus::eNO_STORAGE_MEDIA_ERR );
		REQUIRE( errorMsg == "noerror" );
		REQUIRE( mp == &mp_apple_ );
	}

	SECTION( "Write value - error cases" )
	{
		const char* json   = "{name:\"APPLE\", val:\"abc\"}";
		bool        result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );

		errorMsg = "noerror";
		json     = "{name:\"APPLE\"}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );

		errorMsg = "noerror";
		json     = "{namex:\"APPLE\"}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );

		errorMsg = "noerror";
		json     = "{name:\"APPLE\", val:123}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );

		errorMsg = "noerror";
		json     = "{name:\"APPLE\", val:\"eBIRDS\"}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );
		REQUIRE( errorMsg != "noerror" );

		errorMsg = "noerror";
		json     = "{name:\"APPLE\", val:2.2e10}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg =[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );
		REQUIRE( errorMsg != "noerror" );

		errorMsg = "noerror";
		json     = "{name:\"BOB\", invalid:1}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );
	}



	SECTION( "Set Invalid" )
	{
		seqNum = mp_apple_.write( Persistence::Record::ServerStatus::eNO_STORAGE_WRONG_SCHEMA );
		const char* json = "{name:\"APPLE\", val:\"ePIGS\", invalid:1}";
		bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( seqNum2 == seqNum + 1 );
		Persistence::Record::ServerStatus value = Persistence::Record::ServerStatus::eUNKNOWN;
		int8_t valid = mp_apple_.read( value, &seqNum );
		REQUIRE( seqNum == seqNum2 );
		REQUIRE( ModelPoint::IS_VALID( valid ) == false );
		REQUIRE( errorMsg == "noerror" );
		REQUIRE( mp == &mp_apple_ );
	}

	SECTION( "lock..." )
	{
		const char* json = "{name:\"APPLE\", val:\"eCLOSING\", locked:true}";
		bool result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		Persistence::Record::ServerStatus value = Persistence::Record::ServerStatus::eUNKNOWN;
		int8_t   valid = mp_apple_.read( value );
		REQUIRE( ModelPoint::IS_VALID( valid ) == true );
		REQUIRE( errorMsg == "noerror" );
		REQUIRE( mp_apple_.isLocked() == true );
		REQUIRE( value == +Persistence::Record::ServerStatus::eCLOSING );

		json   = "{name:\"APPLE\", invalid:21, locked:false}";
		result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( mp_apple_.isNotValid() == true );
		REQUIRE( mp_apple_.isLocked() == false );
		REQUIRE( mp_apple_.getValidState() == 21 );

		json   = "{name:\"APPLE\", val:\"eCLOSED\", locked:true}";
		result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( mp_apple_.isLocked() == true );
		valid = mp_apple_.read( value );
		REQUIRE( ModelPoint::IS_VALID( valid ) == true );
		REQUIRE( value == +Persistence::Record::ServerStatus::eCLOSED );

		json   = "{name:\"APPLE\", val:\"eOPENING\" }";
		result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( mp_apple_.isLocked() == true );
		valid = mp_apple_.read( value );
		REQUIRE( ModelPoint::IS_VALID( valid ) == true );
		REQUIRE( value == +Persistence::Record::ServerStatus::eCLOSED );

		json   = "{name:\"APPLE\", locked:false}";
		result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		valid = mp_apple_.read( value );
		REQUIRE( ModelPoint::IS_VALID( valid ) == true );
		REQUIRE( value == +Persistence::Record::ServerStatus::eCLOSED );
		REQUIRE( mp_apple_.isLocked() == false );
	}

	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


///////////////////////////////////////////////////////////////////////////////

static Cpl::Dm::MailboxServer     t1Mbox_;

TEST_CASE( "serverstatus-observer", "[serverstatus-observer]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SERVERSTATUS-OBSERVER test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
    ViewerMpServerStatus viewer1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );

    // Open, write a value, wait for Viewer to see the change, then close
    mp_apple_.write( Persistence::Record::ServerStatus::eRUNNING, ModelPoint::eUNLOCK );
    viewer1.open();
    uint16_t seqNum = mp_apple_.write( Persistence::Record::ServerStatus::eOPENING );
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