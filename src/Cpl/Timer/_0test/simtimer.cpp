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
#include "Cpl/Itc/MailboxServer.h"

/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_simtimer(void) {}



/// 
using namespace Cpl::Timer;

////////////////////////////////////////////////////////////////////////////////
/* The test app consists of 3 threads:
   - Two client threads, one contains a viewer, the other contains a writer
   - One model thread that 'owns' the data being viewed/written
*/

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "simtimer", "[simtimer]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Construct 'fast tick' Mailboxes
    Cpl::Itc::MailboxServer  flowers( FAST_TICKSRC_MS_PER_TICK );
    Cpl::Itc::MailboxServer  trees( FAST_TICKSRC_MS_PER_TICK );

    // Construct 'slow tick' mailboxes
    Cpl::Itc::MailboxServer  fruits( SLOW_TICKSRC_MS_PER_TICK );


    // Create my Application objects
    AppObject appRose(   "rose",   flowers, flowers, ROSE_T1,   ROSE_T2 );
    AppObject appDaisy(  "daisy",  flowers, flowers, DAISY_T1,  DAISY_T2 );
    AppObject appOak(    "oak",    trees,   trees,   OAK_T1,    OAK_T2 );
    AppObject appPine(   "pine",   trees,   trees,   PINE_T1,   PINE_T2 );
    AppObject appApple(  "apple",  fruits,  fruits,  APPLE_T1,  APPLE_T2 );
    AppObject appOrange( "orange", fruits,  fruits,  ORANGE_T1, ORANGE_T2 );


    // Create my test sequencer
    Master  masterRun( NUM_SEQ_, DELAY_, appApple, appRose, appDaisy, appOak, appPine, appOrange, Cpl::System::Thread::getCurrent() );

    // Create all of the threads
    Cpl::System::Thread* t1  = Cpl::System::Thread::create( fruits,     "FRUITS", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_RAISE );
    Cpl::System::Thread* t2  = Cpl::System::Thread::create( trees,      "TREES", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_RAISE );
    Cpl::System::Thread* t3  = Cpl::System::Thread::create( flowers,    "FLOWERS", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_RAISE );
    Cpl::System::Thread* t10 = Cpl::System::Thread::create( masterRun,  "MASTER" );

    // Give time for all of threads to be created before starting the test sequence
    Cpl::System::Api::sleep(200);
    t10->signal();

    // Validate result of each sequence
    int i;
    for(i=0; i< NUM_SEQ_; i++ )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "@@ TICK SOURCE: Starting sequence# %d...", i+1 ));
        Cpl::System::SimTick::advance( DELAY_ );
        Cpl::System::Api::sleepInRealTime( 100 );
        Cpl::System::SimTick::advance( 1000 );
        Cpl::System::Api::sleepInRealTime( 100 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "  @@ TICK SOURCE: pause before checking result for seq# %d.  Seq completd at sim tick count of: %lu", i+1, Cpl::System::SimTick::current() ));
        Cpl::System::Thread::wait();


        unsigned long maxCount = MAX_COUNT_( (unsigned long)appApple.m_deltaTime1, APPLE_T1 );
        appApple.displayTimer1( maxCount );
        REQUIRE( appApple.m_count1 <= maxCount ); 
        REQUIRE( appApple.m_minTime1 >=  appApple.m_timer1Duration ); 
        maxCount = MAX_COUNT_( (unsigned long)appApple.m_deltaTime2, APPLE_T2 );
        appApple.displayTimer2( maxCount );
        REQUIRE( appApple.m_count2 <= maxCount ); 
        REQUIRE( appApple.m_minTime2 >=  appApple.m_timer2Duration ); 

        maxCount = MAX_COUNT_( (unsigned long)appOrange.m_deltaTime1, ORANGE_T1 );
        appOrange.displayTimer1( maxCount );
        REQUIRE( appOrange.m_count1 <= maxCount ); 
        REQUIRE( appOrange.m_minTime1 >=  appOrange.m_timer1Duration ); 
        maxCount = MAX_COUNT_( (unsigned long)appOrange.m_deltaTime2, ORANGE_T2 );
        appOrange.displayTimer2( maxCount );
        REQUIRE( appOrange.m_count2 <= maxCount ); 
        REQUIRE( appOrange.m_minTime2 >=  appOrange.m_timer2Duration ); 

        maxCount = MAX_COUNT_( (unsigned long)appPine.m_deltaTime1, PINE_T1 );
        appPine.displayTimer1( maxCount );
        REQUIRE( appPine.m_count1 <= maxCount ); 
        REQUIRE( appPine.m_minTime1 >=  appPine.m_timer1Duration ); 
        maxCount = MAX_COUNT_( (unsigned long)appPine.m_deltaTime2, PINE_T2 );
        appPine.displayTimer2( maxCount );
        REQUIRE( appPine.m_count2 <= maxCount ); 
        REQUIRE( appPine.m_minTime2 >=  appPine.m_timer2Duration ); 
        
        maxCount = MAX_COUNT_( (unsigned long)appOak.m_deltaTime1, OAK_T1 );
        appOak.displayTimer1( maxCount );
        REQUIRE( appOak.m_count1 <= maxCount ); 
        REQUIRE( appOak.m_minTime1 >=  appOak.m_timer1Duration ); 
        maxCount = MAX_COUNT_( (unsigned long)appOak.m_deltaTime2, OAK_T2 );
        appOak.displayTimer2( maxCount );
        REQUIRE( appOak.m_count2 <= maxCount ); 
        REQUIRE( appOak.m_minTime2 >=  appOak.m_timer2Duration ); 
        
        maxCount = MAX_COUNT_( (unsigned long)appRose.m_deltaTime1, ROSE_T1 );
        appRose.displayTimer1( maxCount );
        REQUIRE( appRose.m_count1 <= maxCount ); 
        REQUIRE( appRose.m_minTime1 >=  appRose.m_timer1Duration ); 
        maxCount = MAX_COUNT_( (unsigned long)appRose.m_deltaTime2, ROSE_T2 );
        appRose.displayTimer2( maxCount );
        REQUIRE( appRose.m_count2 <= maxCount ); 
        REQUIRE( appRose.m_minTime2 >=  appRose.m_timer2Duration ); 
        
        maxCount = MAX_COUNT_( (unsigned long)appDaisy.m_deltaTime1, DAISY_T1 );
        appDaisy.displayTimer1( maxCount );
        REQUIRE( appDaisy.m_count1 <= maxCount ); 
        REQUIRE( appDaisy.m_minTime1 >=  appDaisy.m_timer1Duration ); 
        maxCount = MAX_COUNT_( (unsigned long)appDaisy.m_deltaTime2, DAISY_T2 );
        appDaisy.displayTimer2( maxCount );
        REQUIRE( appDaisy.m_count2 <= maxCount ); 
        REQUIRE( appDaisy.m_minTime2 >=  appDaisy.m_timer2Duration ); 

        t10->signal();
        }

    // Wait for all of the sequences to complete
    Cpl::System::SimTick::advance( 10 );
    Cpl::System::Thread::wait();
    
    // Shutdown threads
    masterRun.pleaseStop();  
    fruits.pleaseStop();      
    trees.pleaseStop();     
    flowers.pleaseStop();       
    Cpl::System::SimTick::advance( 50 );

    Cpl::System::Api::sleep(100); // allow time for threads to stop
    REQUIRE( t1->isRunning()  == false );
    REQUIRE( t2->isRunning()  == false );
    REQUIRE( t3->isRunning()  == false );
    REQUIRE( t10->isRunning() == false );

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Thread::destroy( *t2 );
    Cpl::System::Thread::destroy( *t3 );
    Cpl::System::Thread::destroy( *t10);

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
    }
