/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/BitArray16.h"
#include <string.h>

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"

#define INITIAL_VALUE       0xAA

////////////////////////////////////////////////////////////////////////////////
using namespace Cpl::Dm;

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::BitArray16       mp_apple_( modelDb_, "APPLE" );
static Mp::BitArray16       mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );


////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "BitArray16" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::DString  errorMsg( "noerror", 1024 );
    char                string[MAX_STR_LENG + 1];
    bool                truncated;
    bool                valid;
    uint16_t            value;

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == sizeof( value ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( value ) + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s]", mpType) );
        REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::BitArray16" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        // Start with MP in the invalid state
        mp_apple_.setInvalid();

        mp_apple_.increment( 2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 2 );  // By design the invalid MP has a 'data value' of zero
        mp_apple_.write( 1 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 );

        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == INITIAL_VALUE );

        mp_orange_.setBit( 8 );
        REQUIRE( mp_orange_.read( value ) );
        REQUIRE( value == 0x1AA );

        mp_orange_.clearBit( 1 );
        REQUIRE( mp_orange_.read( value ) );
        REQUIRE( value == 0x1A8  );

        mp_orange_.flipBit( 3 );
        REQUIRE( mp_orange_.read( value ) );
        REQUIRE( value == 0x1A0  );

        mp_orange_.setBitsByMask( 0x5501 );
        REQUIRE( mp_orange_.read( value ) );
        REQUIRE( value == 0x55A1);

        mp_orange_.clearBitsByMask( 0xF000 );
        REQUIRE( mp_orange_.read( value ) );
        REQUIRE( value == 0x05A1  );

        mp_orange_.flipBitsByMask( 0x7701 );
        REQUIRE( mp_orange_.read( value ) );
        REQUIRE( value == 0x72A0  );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_apple_.write( 0x1082 );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( STRCMP( doc["val"], "0001000010000010") );
    }

    SECTION( "fromJSON" )
    {
        // Start with MP in the invalid state
        mp_apple_.setInvalid();

        const char* json = "{name:\"APPLE\", val:\"1000000100000010\"}";
        bool result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 0x8102 );

        json = "{name:\"APPLE\", val:123}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        json = "{name:\"APPLE\", val:123}";
        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

