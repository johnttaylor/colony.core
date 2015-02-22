#ifndef Common_h_
#define Common_h_
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

#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/ElaspedTime.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Timer/Local.h"


#define SECT_                       "_0test"

#define NUM_SEQ_                    3
#define TOLERANCE_                  2
#define DELAY_                      1050

                                  
#define FAST_TICKSRC_MS_PER_TICK    1
#define SLOW_TICKSRC_MS_PER_TICK    10
                                  
#define ROSE_T1                     10      // Tick Hz 1000 (1ms)  
#define ROSE_T2                     23      // Tick Hz 1000 (1ms)  
#define DAISY_T1                    11      // Tick Hz 1000 (1ms)  
#define DAISY_T2                    47      // Tick Hz 1000 (1ms)  
#define OAK_T1                      60      // Tick Hz 1000 (1ms)  
#define OAK_T2                      125     // Tick Hz 1000 (1ms)  
#define PINE_T1                     50      // Tick Hz 1000 (1ms)
#define PINE_T2                     300     // Tick Hz 1000 (1ms)
#define APPLE_T1                    50      // Tick Hz 20   (50ms)
#define APPLE_T2                    300     // Tick Hz 20   (50ms)
#define ORANGE_T1                   60      // Tick Hz 20   (50ms)
#define ORANGE_T2                   125     // Tick Hz 20   (50ms)



#define MAX_COUNT_(ttime, tdur )          (ttime/tdur)




////////////////////////////////////////////////////////////////////////////////
class AppObject: public Cpl::Itc::CloseSync
{
public:
    ///
    const char*                         m_name;
    ///
    bool                                m_opened;
    ///
    unsigned long                       m_timer1Duration;
    ///
    Cpl::Timer::Local<AppObject>        m_timer1;
    ///
    unsigned long                       m_count1;
    ///
    unsigned long                       m_startTime1;
    ///
    unsigned long                       m_minTime1;
    ///
    unsigned long                       m_maxTime1;
    ///
    unsigned long                       m_sumTime1;

    ///
    unsigned long                       m_timer2Duration;
    ///
    Cpl::Timer::Local<AppObject>        m_timer2;
    ///
    unsigned long                       m_count2;

    ///
    unsigned long                       m_timeMark1;
    ///
    unsigned long                       m_timeMark2;
    ///
    unsigned long                       m_deltaTime1;
    ///
    unsigned long                       m_deltaTime2;
    ///
    unsigned long                       m_startTime2;
    ///
    unsigned long                       m_minTime2;
    ///
    unsigned long                       m_maxTime2;
    ///
    unsigned long                       m_sumTime2;


    /// Constructor
    AppObject( const char* name, Cpl::Itc::PostApi& myMailbox, Cpl::Timer::CounterSource& timingSource, unsigned long timer1Duration, unsigned long timer2Duration )
    :Cpl::Itc::CloseSync( myMailbox ),
     m_name(name),
     m_opened( false ),
     m_timer1Duration( timer1Duration ),
     m_timer1( timingSource, *this, &AppObject::timer1Expired ),
     m_timer2Duration( timer2Duration ),
     m_timer2( timingSource, *this, &AppObject::timer2Expired )
        {
        }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("OPENING AppObject: %s....", m_name ));
        if ( m_opened )
            {
            Cpl::System::FatalError::logf( "OPENING AppObect (%s) more than ONCE", m_name );
            }
        
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Starting timers (%lu, %lu) for: %s",  m_timer1Duration, m_timer2Duration, m_name ));
        m_opened   = true;
        m_count1   = 0;
        m_minTime1 = (unsigned long)(-1);
        m_maxTime1 = 0;
        m_sumTime1 = 0;
        m_count2   = 0;
        m_minTime2 = (unsigned long)(-1);
        m_maxTime2 = 0;
        m_sumTime2 = 0;

        m_timeMark1 = m_startTime1 = Cpl::System::ElaspedTime::milliseconds();
        m_timer1.start( m_timer1Duration );
        m_timeMark2 = m_startTime2 = Cpl::System::ElaspedTime::milliseconds();
        m_timer2.start( m_timer2Duration );

        msg.returnToSender();
        }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("CLOSING AppObject: %s....", m_name ));

        if ( !m_opened )
            {
            Cpl::System::FatalError::logf( "CLOSING AppObect (%s) more than ONCE", m_name );
            }

        m_opened    = false;
        m_timer1.stop();
        m_deltaTime1 = Cpl::System::ElaspedTime::deltaMilliseconds(m_timeMark1);
        m_timer2.stop();
        m_deltaTime2 = Cpl::System::ElaspedTime::deltaMilliseconds(m_timeMark1);

		msg.returnToSender();
        }

public:
    ///
    void timer1Expired( void )
        {
        // Capture stats
        unsigned elasped = Cpl::System::ElaspedTime::deltaMilliseconds(m_startTime1);
        if ( elasped < m_minTime1 )
            {
            m_minTime1 = elasped;
            }
        if ( elasped > m_maxTime1 )
            {
            m_maxTime1 = elasped;
            }
        m_sumTime1 += elasped;

        m_count1++;
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "(%s)::Timer 1::Expired.  Total count=%lu (%lu)", m_name, m_count1, m_timer1Duration ));
        m_startTime1 = Cpl::System::ElaspedTime::milliseconds();
        m_timer1.start( m_timer1Duration );        
        }

    void timer2Expired( void )
        {
        // Capture stats
        unsigned elasped = Cpl::System::ElaspedTime::deltaMilliseconds(m_startTime2);
        if ( elasped < m_minTime2 )
            {
            m_minTime2 = elasped;
            }
        if ( elasped > m_maxTime2 )
            {
            m_maxTime2 = elasped;
            }
        m_sumTime2 += elasped;

        m_count2++;
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "(%s)::Timer 2 ::Expired.  Total count=%lu (%lu)", m_name, m_count2, m_timer2Duration ));
        m_startTime2 = Cpl::System::ElaspedTime::milliseconds();
        m_timer2.start( m_timer2Duration );        
        }

public:
    void displayTimer1( unsigned long maxCount )                                            
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "%s#1: delta=%lu, dur=%lu --> cnt (%lu) <=? %lu. min=%lu, max=%lu, avg=%.2f", m_name, m_deltaTime1, m_timer1Duration, m_count1, maxCount, m_minTime1, m_maxTime1, m_sumTime1 / (double)m_count1 ));
        }

    void displayTimer2( unsigned long maxCount )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "%s#2: delta=%lu, dur=%lu --> cnt (%lu) <=? %lu. min=%lu, max=%lu, avg=%.2f", m_name, m_deltaTime2, m_timer2Duration, m_count2, maxCount, m_minTime2, m_maxTime2, m_sumTime2 / (double)m_count2 ));
        }

};


////////////////////////////////////////////////////////////////////////////////
class Master: public Cpl::System::Runnable
{
public: 
    ///
    unsigned                m_numSeqs;
    ///
    unsigned long           m_delayMsecs;
    ///
    AppObject&              m_appObj1;
    ///
    AppObject&              m_appObj2;
    ///
    AppObject&              m_appObj3;
    ///
    AppObject&              m_appObj4;
    ///
    AppObject&              m_appObj5;
    ///
    AppObject&              m_appObj6;
    ///
    Cpl::System::Signable&  m_waiter;
    
public:
    ///
    Master( unsigned numSeqs, unsigned long delayMsecs, AppObject& appObj1, AppObject& appObj2, AppObject& appObj3, AppObject& appObj4, AppObject& appObj5, AppObject& appObj6, Cpl::System::Signable& waiter )
        :m_numSeqs(numSeqs),
         m_delayMsecs(delayMsecs),
         m_appObj1(appObj1),
         m_appObj2(appObj2),
         m_appObj3(appObj3),
         m_appObj4(appObj4),
         m_appObj5(appObj5),
         m_appObj6(appObj6),
         m_waiter(waiter)
            {
            }


public:
    ///
    void appRun()
        {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, Cpl::System::Thread::getCurrent().getName() );

        // Wait for signal to start the test
        Cpl::System::Thread::wait();

        // Run the test sequence
        unsigned i;
        for(i=0; i<m_numSeqs; i++)
            {
            CPL_SYSTEM_SIM_TICK_TOP_LEVEL_WAIT();

            CPL_SYSTEM_TRACE_MSG( SECT_, ( "----- Starting Sequence#: %d....", i+1 ));
            m_appObj1.open();
            m_appObj2.open();
            m_appObj3.open();
            m_appObj4.open();
            m_appObj5.open();
            m_appObj6.open();

            // Delay to let the timer expire
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "----- Waiting for: %lu....", m_delayMsecs));
            Cpl::System::Api::sleep( m_delayMsecs );

            CPL_SYSTEM_TRACE_MSG( SECT_, ( "----- Ending Sequence#: %d.  Closing AppObj1....", i+1 ));
            m_appObj1.close();
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "-----   Closing AppObj2...." ));
            m_appObj2.close();
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "-----   Closing AppObj3...." ));
            m_appObj3.close();
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "-----   Closing AppObj4...." ));
            m_appObj4.close();
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "-----   Closing AppObj5...." ));
            m_appObj5.close();
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "-----   Closing AppObj6...." ));
            m_appObj6.close();

            CPL_SYSTEM_TRACE_MSG( SECT_, ( "-----   One more delay...." ));
            Cpl::System::Api::sleep( 50*3 ); // Delay to allow other threads to run.

            // Handshake with the supervising thread to validate sequence results
            m_waiter.signal(); 
            Cpl::System::Thread::wait();
            }

        m_waiter.signal();
        }

};






#endif // end header latch