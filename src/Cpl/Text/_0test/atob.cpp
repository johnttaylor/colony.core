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
#include "Cpl/Text/atob.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>
#include <stdint.h>
#include "colony_map.h"


/// 
using namespace Cpl::Text;
using namespace Cpl::System;

/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_atob(void) {}




////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "atob", "[atob]" )
    {

    Shutdown_TS::clearAndUseCounter();

    SECTION( "conversions...." )
        {
        const char* strPtr = "12.3";
        const char* endPtr = 0;
        int value;

        bool result = a2i( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2i( value, " 1234 " );
        REQUIRE( value == 1234 );
        REQUIRE( result == false );
        result = a2i( value, " -1234(" );
        REQUIRE( value == -1234 );
        REQUIRE( result == false );
        result = a2i( value, "12.3", 10, ":,." );
        REQUIRE( value == 12 );
        REQUIRE( result == true );
        result = a2i( value, strPtr, 10, ".,", &endPtr );
        REQUIRE( value == 12 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr+2 ); 
        result = a2i( value, "1.23" );
        REQUIRE( value == 1 );
        REQUIRE( result == false );

        result = a2i( value, "0xF" );
        REQUIRE( value == 0 );
        REQUIRE( result == false );
        result = a2i( value, "0xF", 0 );
        REQUIRE( value == 0xF );
        REQUIRE( result == true );

        result = a2i( value, 0 );
        REQUIRE( result == false );
        }

    SECTION( "unsigned..." )
        {
        const char* strPtr = "-123";
        const char* endPtr = 0;
        unsigned value;

        bool result = a2ui( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2ui( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == (unsigned) -123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr+4 );
        result = a2ui( value, 0 );
        REQUIRE( result == false );
        }

    SECTION( "long..." )
        {
        const char* strPtr = "123";
        const char* endPtr = 0;
        long value;

        bool result = a2l( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2l( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr+3 );
        result = a2l( value, 0 );
        REQUIRE( result == false );
        }

    SECTION( "unsigned long..." )
        {
        const char* strPtr = "-123";
        const char* endPtr = 0;
        unsigned long value;

        bool result = a2ul( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2ul( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == (unsigned long)-123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr+4 );
        result = a2ul( value, 0 );
        REQUIRE( result == false );
        }

    SECTION( "long long..." )
        {
        const char* strPtr = "123";
        const char* endPtr = 0;
        long long value;

        bool result = a2ll( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2ll( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr+3 );
        result = a2ll( value, 0 );
        REQUIRE( result == false );
        }

    SECTION( "unsigned long long..." )
        {
        const char* strPtr = "-123";
        const char* endPtr = 0;
        unsigned long long value;

        bool result = a2ull( value, "123" );
        REQUIRE( value == 123u );
        REQUIRE( result == true );
        result = a2ull( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == (unsigned long long) -123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr+4 );
        result = a2ull( value, 0 );
        REQUIRE( result == false );
        }

    SECTION( "double..." )
        {
        const char* strPtr = "-123.32";
        const char* endPtr = 0;
        double value;

        bool result = a2d( value, "123.32" );
        REQUIRE( value == 123.32 );
        REQUIRE( result == true );
        result = a2d( value, strPtr, 0, &endPtr );
        REQUIRE( value == -123.32 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr+7 );
        result = a2d( value, 0 );
        REQUIRE( result == false );
        }

    SECTION( "bool" )
        {
        const char* strPtr = "false";
        const char* endPtr = 0;
        bool value;

        bool result = a2b( value, "T" );
        REQUIRE( value == true );
        REQUIRE( result == true );
        result = a2b( value, strPtr, "true", "false", &endPtr );
        REQUIRE( value == false );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr+5 );
        result = a2b( value, 0 );
        REQUIRE( result == false );
        }

    SECTION( "bool" )
        {
        const char* text = "AA0055BBFC";
        uint8_t buffer[5];

        bool result = asciiHexToBuffer( buffer, text, sizeof(buffer) );
        REQUIRE( result == true );
        REQUIRE( buffer[0] == 0xAA );
        REQUIRE( buffer[1] == 0x00 );
        REQUIRE( buffer[2] == 0x55 );
        REQUIRE( buffer[3] == 0xBB );
        REQUIRE( buffer[4] == 0xFC );

        result = asciiHexToBuffer( buffer, 0, sizeof(buffer) );
        REQUIRE( result == false );
        result = asciiHexToBuffer( 0, text, sizeof(buffer) );
        REQUIRE( result == false );
        result = asciiHexToBuffer( buffer, text, 0 );
        REQUIRE( result == false );

        result = asciiHexToBuffer( buffer, "AA3", sizeof(buffer) );
        REQUIRE( result == false );

        result = asciiHexToBuffer( buffer, text, 3 );
        REQUIRE( result == false );
        }

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
    }
