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

#include "Cpl/Timer/_0test/common.h"
#include "Cpl/Itc/MailboxServer.h"
#include "Bsp/Api.h"

#define REQUIRE(a,b)  if ( !(a) ) Cpl::System::FatalError::logf(b)



/// 
using namespace Cpl::Timer;


////////////////////////////////////////////////////////////////////////////////
namespace {

class TestUnit
{
public:
    ///
    Cpl::Itc::MailboxServer  m_flowers;
    ///
    Cpl::Itc::MailboxServer  m_trees;
    ///
    Cpl::Itc::MailboxServer  m_fruits;

    ///
    AppObject m_appRose;
    ///
    AppObject m_appDaisy;
    ///
    AppObject m_appOak;
    ///
    AppObject m_appPine;
    ///
    AppObject m_appApple;
    ///
    AppObject m_appOrange;


public:
    ///
    TestUnit()
    :m_flowers( FAST_TICKSRC_MS_PER_TICK )
    ,m_trees( FAST_TICKSRC_MS_PER_TICK )
    ,m_fruits( SLOW_TICKSRC_MS_PER_TICK )
    ,m_appRose(   "rose",   m_flowers, m_flowers, ROSE_T1,   ROSE_T2 )
    ,m_appDaisy(  "daisy",  m_flowers, m_flowers, DAISY_T1,  DAISY_T2 )
    ,m_appOak(    "oak",    m_trees,   m_trees,   OAK_T1,    OAK_T2 )
    ,m_appPine(   "pine",   m_trees,   m_trees,   PINE_T1,   PINE_T2 )
    ,m_appApple(  "apple",  m_fruits,  m_fruits,  APPLE_T1,  APPLE_T2 )
    ,m_appOrange( "orange", m_fruits,  m_fruits,  ORANGE_T1, ORANGE_T2 )
        {
        }
};


class Checker: public Cpl::System::Runnable
{

public:
    ///
    Checker()
        {
        }

public:
    ///
    void appRun()
        {
        CPL_SYSTEM_TRACE_FUNC( SECT_ );
       
        for(;;)
            {
            Bsp_Api_turnOn_debug1();    
            
            // Create my test sequencer
            TestUnit*            testPtr         = new(std::nothrow) TestUnit();
            Master*              masterRunPtr    = new(std::nothrow) Master( NUM_SEQ_, DELAY_, testPtr->m_appApple, testPtr->m_appRose, testPtr->m_appDaisy, testPtr->m_appOak, testPtr->m_appPine, testPtr->m_appOrange, Cpl::System::Thread::getCurrent() );
            Cpl::System::Thread* t1Ptr           = Cpl::System::Thread::create( testPtr->m_fruits,   "FRUITS" );
            Cpl::System::Thread* t2Ptr           = Cpl::System::Thread::create( testPtr->m_trees,    "TREES" );
            Cpl::System::Thread* t3Ptr           = Cpl::System::Thread::create( testPtr->m_flowers,  "FLOWERS" );
            Cpl::System::Thread* masterThreadPtr = Cpl::System::Thread::create( *masterRunPtr,       "MASTER" );

            // Give time for all of threads to be created before starting the test sequence
            Cpl::System::Api::sleep(100);
            masterThreadPtr->signal();

            CPL_SYSTEM_TRACE_MSG( SECT_, ( "Starting the sequence...." ));
            int i;
            for(i=0; i< NUM_SEQ_; i++ )
                {
                Bsp_Api_toggle_debug2();    

                Cpl::System::Thread::wait();
                Cpl::System::Api::sleep(50);

                unsigned long maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appApple.m_deltaTime1, APPLE_T1 );
                testPtr->m_appApple.displayTimer1( maxCount );
                REQUIRE( testPtr->m_appApple.m_count1 <= maxCount, "SEQ: 01" ); 
                REQUIRE( testPtr->m_appApple.m_minTime1 >=  testPtr->m_appApple.m_timer1Duration, "SEQ: 02" ); 
                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appApple.m_deltaTime2, APPLE_T2 );
                testPtr->m_appApple.displayTimer2( maxCount );
                REQUIRE( testPtr->m_appApple.m_count2 <= maxCount, "SEQ: 03" ); 
                REQUIRE( testPtr->m_appApple.m_minTime2 >=  testPtr->m_appApple.m_timer2Duration, "SEQ: 04" ); 

                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appOrange.m_deltaTime1, ORANGE_T1 );
                testPtr->m_appOrange.displayTimer1( maxCount );
                REQUIRE( testPtr->m_appOrange.m_count1 <= maxCount, "SEQ: 05" ); 
                REQUIRE( testPtr->m_appOrange.m_minTime1 >=  testPtr->m_appOrange.m_timer1Duration, "SEQ: 06" ); 
                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appOrange.m_deltaTime2, ORANGE_T2 );
                testPtr->m_appOrange.displayTimer2( maxCount );
                REQUIRE( testPtr->m_appOrange.m_count2 <= maxCount, "SEQ: 07" ); 
                REQUIRE( testPtr->m_appOrange.m_minTime2 >=  testPtr->m_appOrange.m_timer2Duration, "SEQ: 08" ); 

                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appPine.m_deltaTime1, PINE_T1 );
                testPtr->m_appPine.displayTimer1( maxCount );
                REQUIRE( testPtr->m_appPine.m_count1 <= maxCount, "SEQ: 09" ); 
                REQUIRE( testPtr->m_appPine.m_minTime1 >=  testPtr->m_appPine.m_timer1Duration, "SEQ: 10" ); 
                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appPine.m_deltaTime2, PINE_T2 );
                testPtr->m_appPine.displayTimer2( maxCount );
                REQUIRE( testPtr->m_appPine.m_count2 <= maxCount, "SEQ: 11" ); 
                REQUIRE( testPtr->m_appPine.m_minTime2 >=  testPtr->m_appPine.m_timer2Duration, "SEQ: 12" ); 
        
                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appOak.m_deltaTime1, OAK_T1 );
                testPtr->m_appOak.displayTimer1( maxCount );
                REQUIRE( testPtr->m_appOak.m_count1 <= maxCount, "SEQ: 13" ); 
                REQUIRE( testPtr->m_appOak.m_minTime1 >=  testPtr->m_appOak.m_timer1Duration, "SEQ: 14" ); 
                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appOak.m_deltaTime2, OAK_T2 );
                testPtr->m_appOak.displayTimer2( maxCount );
                REQUIRE( testPtr->m_appOak.m_count2 <= maxCount, "SEQ: 15" ); 
                REQUIRE( testPtr->m_appOak.m_minTime2 >=  testPtr->m_appOak.m_timer2Duration, "SEQ: 16" ); 
        
                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appRose.m_deltaTime1, ROSE_T1 );
                testPtr->m_appRose.displayTimer1( maxCount );
                REQUIRE( testPtr->m_appRose.m_count1 <= maxCount, "SEQ: 17" ); 
                REQUIRE( testPtr->m_appRose.m_minTime1 >=  testPtr->m_appRose.m_timer1Duration, "SEQ: 18" ); 
                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appRose.m_deltaTime2, ROSE_T2 );
                testPtr->m_appRose.displayTimer2( maxCount );
                REQUIRE( testPtr->m_appRose.m_count2 <= maxCount, "SEQ: 19" ); 
                REQUIRE( testPtr->m_appRose.m_minTime2 >=  testPtr->m_appRose.m_timer2Duration, "SEQ: 20" ); 
        
                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appDaisy.m_deltaTime1, DAISY_T1 );
                testPtr->m_appDaisy.displayTimer1( maxCount );
                REQUIRE( testPtr->m_appDaisy.m_count1 <= maxCount, "SEQ: 21" ); 
                REQUIRE( testPtr->m_appDaisy.m_minTime1 >=  testPtr->m_appDaisy.m_timer1Duration, "SEQ: 22" ); 
                maxCount = MAX_COUNT_( (unsigned long)testPtr->m_appDaisy.m_deltaTime2, DAISY_T2 );
                testPtr->m_appDaisy.displayTimer2( maxCount );
                REQUIRE( testPtr->m_appDaisy.m_count2 <= maxCount, "SEQ: 23" ); 
                REQUIRE( testPtr->m_appDaisy.m_minTime2 >=  testPtr->m_appDaisy.m_timer2Duration, "SEQ: 24" ); 

                masterThreadPtr->signal();
                }

            // Wait for all of the sequences to complete
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "Sequence Complete: Waiting for all threads to finish" ));
            Bsp_Api_turnOff_debug2();    
            Cpl::System::Thread::wait();

            // Shutdown threads
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "Shutting down threads..." ));
            Bsp_Api_turnOff_debug1();    
            testPtr->m_fruits.pleaseStop();
            testPtr->m_trees.pleaseStop();
            testPtr->m_flowers.pleaseStop();
            Cpl::System::Api::sleep(50); // allow time for threads to stop
            REQUIRE( t1Ptr->isRunning() == false, "Shutdown: 1" );
            REQUIRE( t2Ptr->isRunning() == false, "Shutdown: 2" );
            REQUIRE( t3Ptr->isRunning() == false, "Shutdown: 3" );
            REQUIRE( masterThreadPtr->isRunning() == false, "Shutdown: 4" );

            Cpl::System::Thread::destroy( *t1Ptr );
            Cpl::System::Thread::destroy( *t2Ptr );
            Cpl::System::Thread::destroy( *t3Ptr );
            Cpl::System::Thread::destroy( *masterThreadPtr );
            delete masterRunPtr;
            delete testPtr;
            
            // Test done -->flash LED to indicate succesful test (i.e. not stuck in the FatalError handler)
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "All threads to destroyed!" ));
            for(i=0; i<4*2; i++)
                {
                Bsp_Api_toggle_debug1();    
                Cpl::System::Api::sleep(250);
                }
            }
        }
};

};  // end namespace


////////////////////////////////////////////////////////////////////////////////

static Checker checker_;

////////////////////////////////////////////////////////////////////////////////
void runtests( void )
    {
    Cpl::System::Thread::create( checker_, "CHECKER" );
 
    // Start the schedular
    Cpl::System::Api::enableScheduling();
    }
