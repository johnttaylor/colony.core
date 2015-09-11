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

#include "Cpl/TShell/Dac/MakerWithScripting.h"

#include "Cpl/TShell/Dac/Cmd/Help.h"
#include "Cpl/TShell/Dac/Cmd/Bye.h"
#include "Cpl/TShell/Dac/Cmd/Trace.h"
#include "Cpl/TShell/Dac/Cmd/Set.h"
#include "Cpl/TShell/Dac/Cmd/Print.h"
#include "Cpl/TShell/Dac/Cmd/TPrint.h"
#include "Cpl/TShell/Dac/Cmd/Try.h"
#include "Cpl/TShell/Dac/Cmd/Inc.h"
#include "Cpl/TShell/Dac/Cmd/Loop.h"
#include "Cpl/TShell/Dac/Cmd/Exe.h"
#include "Cpl/TShell/Dac/Cmd/Tokenize.h"

#include "Rte/TShell/Dac/Cmd/RteRead.h"


////////////////////////////////////////////////////////////////////////////////

#ifndef MAXVARS_
#define MAXVARS_                4
#endif

#ifndef MAX_CMD_BUFFER_
#define MAX_CMD_BUFFER_         64
#endif


extern Cpl::Container::Map<Cpl::TShell::Dac::Command>   cmdlist;
extern Cpl::Container::Map<Rte::TShell::Dac::Point>     pointList;

static Cpl::TShell::Dac::MakerWithScripting<MAXVARS_,MAX_CMD_BUFFER_> cmdProcessor_( "invoke_special_static_constructor", cmdlist );

static Cpl::TShell::Dac::Cmd::Help                      helpCmd_( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::Bye                       byeCmd_( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::Trace                     traceCmd_( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::Set                       setCmd_( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::Print                     printCmd_( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::TPrint                    tprintCmd_( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::Try                       tryCmd( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::Inc                       incCmd( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::Loop                      loopCmd( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::Exe                       ExeCmd( cmdlist, "invoke_special_static_constructor" );
static Cpl::TShell::Dac::Cmd::Tokenize                  TokenizeCmd( cmdlist, "invoke_special_static_constructor" );

static Rte::TShell::Dac::Cmd::RteRead                   readCmd( cmdlist, pointList, "invoke_special_static_constructor" );




