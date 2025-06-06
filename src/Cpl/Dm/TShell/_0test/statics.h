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

#include "helpers.h"
#include "Cpl/TShell/Maker.h"

#include "Cpl/Dm/TShell/Dm.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Int32.h"



////////////////////////////////////////////////////////////////////////////////

extern Cpl::Container::SList<Cpl::TShell::Command>   cmdlist;

static Cpl::TShell::Maker cmdProcessor_( cmdlist );

static Cpl::TShell::Cmd::Help    helpCmd_( cmdlist );
static Cpl::TShell::Cmd::Bye     byeCmd_( cmdlist );
static Cpl::TShell::Cmd::Trace   traceCmd_( cmdlist );
static Cpl::TShell::Cmd::TPrint  tprintCmd_( cmdlist );


static Apple   mockApp;
static Bob     bobCmd( cmdlist, mockApp );


// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase   modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

static Cpl::Dm::TShell::Dm		dmCmd_( cmdlist, modelDb_, "dm" );

// Allocate my Model Points
static Cpl::Dm::Mp::Int32       mp_apple_( modelDb_, "APPLE"  );

static Cpl::Dm::Mp::Int32       mp_orange_( modelDb_, "ORANGE", 32 );
