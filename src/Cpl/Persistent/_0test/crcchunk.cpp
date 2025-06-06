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
#include "Cpl/System/Thread.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Api.h"
#include "Cpl/Persistent/CrcChunk.h"
#include "Cpl/Persistent/RecordServer.h"
#include "Cpl/Persistent/FileAdapter.h"
#include "Cpl/Persistent/NullRegionMedia.h"
#include "Cpl/Io/File/Api.h"
#include "Cpl/Io/File/Output.h"
#include <memory.h>

#define SECT_   "_0test"

using namespace Cpl::Persistent;

//////////////////////////
namespace {

class MyUut : public CrcChunk
{
public:
    int m_startCount;
    int m_stopCount;

    MyUut( RegionMedia& r1 ) : CrcChunk( r1 ), m_startCount( 0 ), m_stopCount( 0 )
    {
    }

    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept
    {
        CrcChunk::start( myMbox );
        m_startCount++;
    }

    /// See Cpl::Persistent::Chunk
    void stop() noexcept
    {
        CrcChunk::stop();
        m_stopCount++;
    }
};

class MyPayload : public Payload
{
public:
    char m_buffer[1024];
    int  m_getCount;
    int  m_putCount;
    bool m_putResult;
    const char* m_getString;

    MyPayload( const char* getString ) :m_getCount( 0 ), m_putCount( 0 ), m_putResult( true ), m_getString( getString )
    {
    }

    size_t getData( void* dst, size_t maxDstLen ) noexcept
    {
        size_t len = strlen( m_getString ) + 1;
        CPL_SYSTEM_ASSERT( maxDstLen >= len );
        m_getCount++;

        memcpy( dst, m_getString, len );
        return len;
    }

    bool putData( const void* src, size_t srcLen ) noexcept
    {
        CPL_SYSTEM_ASSERT( srcLen <= sizeof( m_buffer ) );
        m_putCount++;
        memcpy( m_buffer, src, srcLen );
        return m_putResult;
    };
};

class MyMockPostApi : public Cpl::Itc::PostApi
{
public:
    MyMockPostApi() {}

    void post( Cpl::Itc::Message& msg ) noexcept {}
    void postSync( Cpl::Itc::Message& msg ) noexcept {}
};

}; // end anonymous namespace

static MyPayload    payload1_( "Hello" );
static MyPayload    payload2_( "World" );
static Record*      records_[] = { 0 };

static RecordServer mockEvents_( records_ );

#define FILE_NAME_REGION1   "region1.nvram"

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "CrcChunk" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "CRC-CHUNK test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    FileAdapter fd1( FILE_NAME_REGION1, 0, 128 );
    MyUut uut( fd1 );

    SECTION( "start/stop" )
    {
        REQUIRE( uut.m_startCount == 0 );
        REQUIRE( uut.m_stopCount == 0 );
        uut.start( mockEvents_ );
        REQUIRE( uut.m_startCount == 1 );
        REQUIRE( uut.m_stopCount == 0 );
        uut.stop();
        REQUIRE( uut.m_startCount == 1 );
        REQUIRE( uut.m_stopCount == 1 );
    }

    SECTION( "Load/update" )
    {
        // Delete files
        Cpl::Io::File::Api::remove( FILE_NAME_REGION1 );
        uut.start( mockEvents_ );

        bool result = uut.loadData( payload1_ );
        REQUIRE( result == false );
        REQUIRE( payload1_.m_putCount == 0 );

        REQUIRE( payload1_.m_getCount == 0 );
        uut.updateData( payload1_ );
        REQUIRE( payload1_.m_getCount == 1 );

        result = uut.loadData( payload1_ );
        REQUIRE( result == true );
        REQUIRE( payload1_.m_putCount == 1 );
        REQUIRE( strcmp( payload1_.m_buffer, payload1_.m_getString ) == 0 );

        REQUIRE( payload2_.m_getCount == 0 );
        uut.updateData( payload2_ );
        REQUIRE( payload2_.m_getCount == 1 );
        REQUIRE( payload1_.m_putCount == 1 );

        result = uut.loadData( payload2_ );
        REQUIRE( result == true );
        REQUIRE( payload2_.m_putCount == 1 );
        REQUIRE( payload1_.m_putCount == 1 );
        printf( "buffer=[%s], expected=[%s]\n", payload2_.m_buffer, payload2_.m_getString );
        REQUIRE( strcmp( payload2_.m_buffer, payload2_.m_getString ) == 0 );

        uut.stop();
    }

    SECTION( "corrupt CRC" )
    {
        uut.start( mockEvents_ );

        // Read the data 
        payload2_.m_putCount = 0;
        payload2_.m_getCount = 0;
        bool result = uut.loadData( payload2_ );
        REQUIRE( result == true );
        REQUIRE( payload2_.m_getCount == 0 );
        REQUIRE( payload2_.m_putCount == 1 );
        REQUIRE( strcmp( payload2_.m_buffer, payload2_.m_getString ) == 0 );

        // Corrupt region 2
        Cpl::Io::File::Output fd( FILE_NAME_REGION1 );
        REQUIRE( fd.isOpened() );
        fd.write( "junk" );
        fd.close();

        // Read the data (should be bad)
        result = uut.loadData( payload2_ );
        REQUIRE( result == false );
        REQUIRE( payload2_.m_getCount == 0 );
        REQUIRE( payload2_.m_putCount == 1 );

        uut.stop();
    }

    SECTION( "Erase" )
    {
        // Delete files
        Cpl::Io::File::Api::remove( FILE_NAME_REGION1 );
        payload1_.m_putCount = 0;
        payload1_.m_getCount = 0;
        uut.start( mockEvents_ );

        bool result = uut.loadData( payload1_ );
        REQUIRE( result == false );
        REQUIRE( payload1_.m_putCount == 0 );

        REQUIRE( payload1_.m_getCount == 0 );
        uut.updateData( payload1_ );
        REQUIRE( payload1_.m_getCount == 1 );

        result = uut.loadData( payload1_ );
        REQUIRE( result == true );
        REQUIRE( payload1_.m_putCount == 1 );
        REQUIRE( strcmp( payload1_.m_buffer, payload1_.m_getString ) == 0 );

        REQUIRE( payload1_.m_getCount == 1 );
        uut.updateData( payload1_, 0, true );   // Erase the record
        REQUIRE( payload1_.m_getCount == 2 );

        result = uut.loadData( payload1_ );
        REQUIRE( result == false );
        REQUIRE( payload1_.m_putCount == 1 );

        uut.stop();
    }

    SECTION( "null media region" )
    {
        NullRegionMedia uutFd( 0, 128 );
        MyUut chunk( uutFd );
        payload1_.m_putCount = 0;
        payload1_.m_getCount = 0;

        bool result = chunk.loadData( payload1_ );
        REQUIRE( result == false );
        REQUIRE( payload1_.m_putCount == 0 );

        REQUIRE( payload1_.m_getCount == 0 );
        uut.updateData( payload1_ );
        REQUIRE( payload1_.m_getCount == 1 );

        result = chunk.loadData( payload1_ );
        REQUIRE( result == false );
        REQUIRE( payload1_.m_putCount == 0 );

        uut.stop();
    }

    SECTION( "null media - start/stop" )
    {
        NullRegionMedia uutFd( 0, 128 );
        MyUut chunk( uutFd );

        REQUIRE( chunk.m_startCount == 0 );
        REQUIRE( chunk.m_stopCount == 0 );
        chunk.start( mockEvents_ );
        REQUIRE( chunk.m_startCount == 1 );
        REQUIRE( chunk.m_stopCount == 0 );
        chunk.stop();
        REQUIRE( chunk.m_startCount == 1 );
        REQUIRE( chunk.m_stopCount == 1 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
