/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Persistence/Record.h"
#include "Cpl/Dm/Mp/Uint32.h"


// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Cpl::Dm::StaticInfo       info_mp_apple_( "APPLE1" );
static Cpl::Dm::Mp::Uint32       mp_apple_( modelDb_, info_mp_apple_ );

static Cpl::Dm::StaticInfo       info_mp_orange_( "ORANGE1" );
static Cpl::Dm::Mp::Uint32       mp_orange_( modelDb_, info_mp_orange_ );

static Cpl::Dm::StaticInfo       info_mp_cherry_( "CHERRY1" );
static Cpl::Dm::Mp::Uint32       mp_cherry_( modelDb_, info_mp_cherry_ );

static Cpl::Dm::StaticInfo       info_mp_plum_( "PLUM1" );
static Cpl::Dm::Uint32       mp_plum_( modelDb_, info_mp_plum_ );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "record" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "RECORD Test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


