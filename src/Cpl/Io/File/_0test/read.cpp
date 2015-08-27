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
#include "Cpl/Io/File/Input.h"
#include "Cpl/Io/LineReader.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"


/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_read(void) {}


#define SECT_     "_0test"


/** Hack to get around the fact the SCM systems will convert newline 
    characters in Text Files based on the target host, i.e. on Windows my 
    newline character will be 2 chars, whereas on posix/linux it will be
    a single character -->hence delta in the file length.
 */
#ifndef TESTING_POSIX
#define TESTINPUT_TXT_FILE_LENGTH   106
#define TESTINPUT_TEXT_HELLO_OFFEST 0x5D
#else
#define TESTINPUT_TXT_FILE_LENGTH   101
#define TESTINPUT_TEXT_HELLO_OFFEST 0x58
#endif

/// 
using namespace Cpl::Io::File;



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "read", "[read]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Reading Lines..." ));
    Input fd( "testinput.txt" );
    Cpl::Io::LineReader reader(fd);
    REQUIRE( fd.isOpened() );

    Cpl::Text::FString<6> line;
    REQUIRE( reader.available() == true );
    reader.readln( line );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    REQUIRE( line == "line 1" );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    REQUIRE( line == "line 2" );
    REQUIRE( reader.readln( line ) );
    REQUIRE( fd.length() == TESTINPUT_TXT_FILE_LENGTH );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    line.removeTrailingSpaces();
    REQUIRE( line.isEmpty() );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    REQUIRE( line == "line 4" );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "line=[%s]", line.getString() ) );
    REQUIRE( line == "line 5" );
    reader.close();
    REQUIRE( fd.isOpened() == false );
    REQUIRE( reader.readln( line ) == false );
    REQUIRE( fd.read( line ) == false );

    //     
    Input fd2( "testinput.txt" );
    REQUIRE( fd2.isOpened() );
    char dummyChar = 29;
    REQUIRE( fd2.setAbsolutePos( TESTINPUT_TEXT_HELLO_OFFEST ) );
    REQUIRE( fd2.length() == TESTINPUT_TXT_FILE_LENGTH );
    REQUIRE( fd2.read(dummyChar) == true );
    REQUIRE( dummyChar == 'A' );

    Cpl::Text::FString<10> buffer("bob");
    REQUIRE( fd2.read(buffer) == true );
    REQUIRE( buffer == "Hello Worl" );

    char myBuffer[10] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
    int  bytesRead    = 1;
    REQUIRE( fd2.available() == true );
    fd2.read( myBuffer, sizeof(myBuffer), bytesRead );
    REQUIRE( bytesRead == 2 );
    REQUIRE( myBuffer[0] == 'd' );
    REQUIRE( myBuffer[1] == '.' );
    REQUIRE( fd2.read( myBuffer, sizeof(myBuffer), bytesRead ) == false );

    fd2.close(); 
    REQUIRE( fd2.isOpened() == false );
    REQUIRE( fd2.read(dummyChar) == false );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
    }
