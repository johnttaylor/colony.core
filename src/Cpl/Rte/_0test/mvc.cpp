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
#include "common.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Api.h"
#include "Cpl/Rte/ModelDatabase.h"
#include "Cpl/Rte/MailboxServer.h"


/// This method is used as part of 'forcing' this object to being actually 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case)
void link_mvc( void ) {}




////////////////////////////////////////////////////////////////////////////////
/* The test app consists of 4 threads:
   - Three client threads, one contains viewers, the other two contain writers
   - One master thread (which the main thread)
*/
// Create my RTE mailboxes
static MailboxServer     viewerMbox_;
static MailboxServer     writerMbox_;
static MailboxServer     readerModifywriterMbox_;

// Allocate/create my Model Database
static ModelDatabase    modelDb_;

// Allocate my Model Points
static StaticInfo       info_mp_apple_( "APPLE" );
static Mp::Uint32       mp_apple_( modelDb_, info_mp_apple_ );

static StaticInfo       info_mp_orange_( "ORANGE" );
static Mp::Uint32       mp_orange_( modelDb_, info_mp_orange_ );

static StaticInfo       info_mp_cherry_( "CHERRY" );
static Mp::Uint32       mp_cherry_( modelDb_, info_mp_cherry_ );

static StaticInfo       info_mp_plum_( "PLUM" );
static Mp::Uint32       mp_plum_( modelDb_, info_mp_plum_ );


#define VIEWER_APPLE1_END_VALUE  101

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "mvc", "[mvc]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    //CPL_SYSTEM_TRACE_ENABLE_SECTION("Cpl::Timer");

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( viewerMbox_, "Viewers" );
    Cpl::System::Thread* t2 = Cpl::System::Thread::create( writerMbox_, "Writers" );
    //Cpl::System::Thread* t3 = Cpl::System::Thread::create( readerModifywriterMbox_, "ReaderModifyWriters" );

    // Create my viewers, writers
    Viewer viewer_apple1( viewerMbox_, Cpl::System::Thread::getCurrent(), mp_apple_, VIEWER_APPLE1_END_VALUE );
    Writer writer_apple1( writerMbox_, Cpl::System::Thread::getCurrent(), mp_apple_, 10, 1, VIEWER_APPLE1_END_VALUE, 1 );
    Writer writer_apple2( writerMbox_, Cpl::System::Thread::getCurrent(), mp_apple_, 0, 1, 100, 1 );

    // Open my viewers, writers
    viewer_apple1.open();
    writer_apple1.open();
    writer_apple2.open();


    // Wait for each viewer, writer, to finish
    Cpl::System::Thread::wait();
    Cpl::System::Thread::wait();
    Cpl::System::Thread::wait();

    REQUIRE( viewer_apple1.m_lastValue == VIEWER_APPLE1_END_VALUE );

    // Close my viewers, writers, 
    viewer_apple1.close();
    writer_apple1.close();
    writer_apple2.close();

    // Shutdown threads
    viewerMbox_.pleaseStop();
    writerMbox_.pleaseStop();
    //readerModifywriterMbox_.pleaseStop();
    Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
    REQUIRE( t1->isRunning() == false );
    REQUIRE( t2->isRunning() == false );
    //REQUIRE( t3->isRunning() == false );

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Thread::destroy( *t2 );
    //Cpl::System::Thread::destroy( *t3 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

