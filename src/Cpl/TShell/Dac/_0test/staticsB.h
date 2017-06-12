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
#include "Cpl/TShell/Dac/Maker.h"


////////////////////////////////////////////////////////////////////////////////

extern Cpl::Container::Map<Cpl::TShell::Dac::Command>   cmdlist;

static Cpl::TShell::Dac::Maker cmdProcessor_( cmdlist );

static Cpl::TShell::Dac::Cmd::Help    helpCmd_( cmdlist, "invoke_special_static_constructor"  );
static Cpl::TShell::Dac::Cmd::Bye     byeCmd_( cmdlist, "invoke_special_static_constructor"  );
static Cpl::TShell::Dac::Cmd::Trace   traceCmd_( cmdlist, "invoke_special_static_constructor"  );


static Apple   mockApp;
static Bob     bobCmd( cmdlist, mockApp, "invoke_special_static_constructor"  );


