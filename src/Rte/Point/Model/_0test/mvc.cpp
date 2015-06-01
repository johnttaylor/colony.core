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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "common.h"


/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_mvc(void) {}






////////////////////////////////////////////////////////////////////////////////
/* The test app consists of 3 threads:
   - Two client threads, one contains a viewer, the other contains a writer
   - One model thread that 'owns' the data being viewed/written
*/


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "mvc", "[mvc]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::System::Thread* modelThreadPtr  = Cpl::System::Thread::create( modelMailbox_,  "MODEL" );
    Cpl::System::Thread* viewerThreadPtr = Cpl::System::Thread::create( viewerMailbox_,  "Viewer" );

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( viewerMailbox_, "Viewer" );
    Cpl::System::Thread* t3 = Cpl::System::Thread::create( modelMailbox_,  "MASTER" );

    // Pause to let the Model thread start
    Cpl::System::Api::sleep(50);

    // BAR1 Query
    Point::QueryBar1 queryBar1(modelBar1_);
    queryBar1.issueQuery();
    traceBar1_( queryBar1, "Q1", "Query - model point in its initial state" );

    /// BAR1 Controller 
    Point::ControllerBar1 controllerBar1(modelBar1_);
    controllerBar1.m_tuple.m_name.setInUse();
    controllerBar1.m_tuple.m_name.setValid();
    controllerBar1.m_tuple.m_name.set("bob");
    controllerBar1.m_tuple.m_enabled.setInUse();
    controllerBar1.m_tuple.m_enabled.setValid();
    controllerBar1.m_tuple.m_enabled.set(true);
    controllerBar1.m_tuple.m_count.setInUse();
    controllerBar1.m_tuple.m_count.setValid();
    controllerBar1.m_tuple.m_count.set(12);
    controllerBar1.updateModel();

    queryBar1.issueQuery();
    traceBar1_( queryBar1, "Q2", "Query - after initial controller write" );

    controllerBar1.m_tuple.m_enabled.set(false);
    controllerBar1.updateModel();

    queryBar1.issueQuery();
    traceBar1_( queryBar1, "Q3", "Query - after update of 'm_enabled'" );
 
    controllerBar1.m_tuple.m_count.set(6);
    controllerBar1.updateModel();

    queryBar1.issueQuery();
    traceBar1_( queryBar1, "Q4", "Query - after update of 'm_count'" );

    
    // Shutdown threads
    viewerMailbox_.pleaseStop();
    modelMailbox_.pleaseStop();
    Cpl::System::Api::sleep(50); // allow time for threads to stop
    REQUIRE( modelThreadPtr->isRunning() == false );
    REQUIRE( viewerThreadPtr->isRunning() == false );

    Cpl::System::Thread::destroy( *modelThreadPtr );
    Cpl::System::Thread::destroy( *viewerThreadPtr );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0 );
    }
