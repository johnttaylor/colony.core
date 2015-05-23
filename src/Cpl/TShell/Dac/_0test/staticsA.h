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

#include "helpers.h"
#include "Cpl/TShell/Dac/MakerWithScripting.h"


////////////////////////////////////////////////////////////////////////////////

extern Cpl::Container::Map<Cpl::TShell::Dac::Command>   cmdlist;

static Cpl::TShell::Dac::MakerWithScripting<MAXVARS_,MAX_CMD_BUFFER_> cmdProcessor_( "invoke_special_static_constructor", cmdlist );

static Cpl::TShell::Dac::Cmd::Help                      helpCmd_( cmdlist );
static Cpl::TShell::Dac::Cmd::Bye                       byeCmd_( cmdlist );
static Cpl::TShell::Dac::Cmd::Trace                     traceCmd_( cmdlist );
static Cpl::TShell::Dac::Cmd::Set                       setCmd_( cmdlist );
static Cpl::TShell::Dac::Cmd::Print                     printCmd_( cmdlist );
static Cpl::TShell::Dac::Cmd::TPrint                    tprintCmd_( cmdlist );
static Cpl::TShell::Dac::Cmd::Try                       tryCmd( cmdlist );
static Cpl::TShell::Dac::Cmd::Inc                       incCmd( cmdlist );
static Cpl::TShell::Dac::Cmd::Loop                      loopCmd( cmdlist );
static Cpl::TShell::Dac::Cmd::Exe                       ExeCmd( cmdlist );
static Cpl::TShell::Dac::Cmd::Tokenize                  TokenizeCmd( cmdlist );


static Apple   mockApp;
static Bob     bobCmd( cmdlist, mockApp );


