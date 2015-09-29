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
#include "Rte/TShell/Dac/_0test/Point/bar1_m.h"
#include "Rte/TShell/Dac/_0test/Point/bar2_m.h"
#include "Rte/TShell/Dac/_0test/Point/bar3_m.h"
#include "Rte/TShell/Dac/_0test/Point/bar4_m.h"
#include "Cpl/TShell/Stdio.h"
#include "Cpl/Itc/MailboxServer.h"






/// 
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

////////////////////////////////////////////////////////////////////////////////
// Mailbox for the server and viewer threads
static Cpl::Itc::MailboxServer modelMailbox_;
static Cpl::Itc::MailboxServer viewerMailbox_;



// Model Points
static  Point::ModelBar1 modelBar1_(modelMailbox_);
static  Point::ModelBar2 modelBar2a_(modelMailbox_);
static  Point::ModelBar2 modelBar2b_(modelMailbox_);
static  Point::ModelBar3 modelBar3_(modelMailbox_);
static  Point::ModelBar4 modelBar4_(modelMailbox_);

// DAC Model Point List
Cpl::Container::Map<Rte::TShell::Dac::Point>     pointList( "invoke_special_static_constructor" );

// Create DAC Model Points
static Rte::TShell::Dac::Point m1_( pointList, modelBar1_,  "bar1" );
static Rte::TShell::Dac::Point m2_( pointList, modelBar2a_, "bar2" );
static Rte::TShell::Dac::Point m3_( pointList, modelBar2b_, "foobar2" );
static Rte::TShell::Dac::Point m4_( pointList, modelBar3_,  "bar3" );
static Rte::TShell::Dac::Point m5_( pointList, modelBar4_,  "bar4" );


// Viewers
ViewerContext   v0_(   "V0", viewerMailbox_, modelBar1_, modelBar2a_, modelBar3_, modelBar4_, true, true, true, true );
ViewerContext   v1_(   "V1", viewerMailbox_, modelBar1_, modelBar2a_, modelBar3_, modelBar4_, true, true, true, true );
ViewerContext   v2_(   "V2", viewerMailbox_, modelBar1_, modelBar2b_, modelBar3_, modelBar4_, true, true, true, true, false, false, false, false );  // Compare using seqnumber
LWViewerContext v3LW_( "V3", viewerMailbox_, modelBar1_, modelBar3_, true, true );
ViewerContext   v4_(   "V4", viewerMailbox_, modelBar1_, modelBar2b_, modelBar3_, modelBar4_, true, true, true, true );

static Viewers viewersCmd( cmdlist, "invoke_special_static_constructor", &v0_, &v1_, &v2_, &v3LW_, &v4_ );


////////////////////////////////////////////////////////////////////////////////
Cpl::TShell::Stdio shell_( cmdProcessor_ );


void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
    {
    // Create Model Thread
    Cpl::System::Thread::create( modelMailbox_,  "MODEL" );
    Cpl::System::Thread::create( viewerMailbox_,  "Viewer" );

    // Start viewers
    v0_.open();
    v1_.open();
    v2_.open();
    v3LW_.open();
    v4_.open();

    // Start the shell
    shell_.launch( infd, outfd );
       
    // Start the schedular
    Cpl::System::Api::enableScheduling();
    }


