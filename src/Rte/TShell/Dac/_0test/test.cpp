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

#include "staticsA.h"
#include "Point/bar1_m.h"
#include "Point/bar2_m.h"
#include "Point/bar3_m.h"
#include "Cpl/TShell/Stdio.h"
#include "Cpl/Itc/MailboxServer.h"



#define SECT_                   "_0test"



/// 
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

////////////////////////////////////////////////////////////////////////////////
// Mailbox for the server 
static Cpl::Itc::MailboxServer modelMailbox_;

// Model Points
static  Point::ModelBar1 modelBar1_(modelMailbox_);
static  Point::ModelBar2 modelBar2a_(modelMailbox_);
static  Point::ModelBar2 modelBar2b_(modelMailbox_);
static  Point::ModelBar3 modelBar3_(modelMailbox_);

// DAC Model Point List
Cpl::Container::Map<Rte::TShell::Dac::Point>     pointList( "invoke_special_static_constructor" );

// Create DAC Model Points
static Rte::TShell::Dac::Point m1_( pointList, modelBar1_,  "bar1" );
static Rte::TShell::Dac::Point m2_( pointList, modelBar2a_, "bar2a" );
static Rte::TShell::Dac::Point m3_( pointList, modelBar2b_, "bar2b" );
static Rte::TShell::Dac::Point m4_( pointList, modelBar3_,  "bar3" );



////////////////////////////////////////////////////////////////////////////////
Cpl::TShell::Stdio shell_( cmdProcessor_ );


void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
    {
    // Create Model Thread
    Cpl::System::Thread* modelThreadPtr  = Cpl::System::Thread::create( modelMailbox_,  "MODEL" );

    // Start the shell
    shell_.launch( infd, outfd );
       
    // Start the schedular
    Cpl::System::Api::enableScheduling();
    }


