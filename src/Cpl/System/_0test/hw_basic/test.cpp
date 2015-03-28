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

#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/ElaspedTime.h"
#include "Cpl/System/Tls.h"



#define SECT_     "_0test"

/// 
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////
namespace {

class MyRunnable: public Runnable
{
public:
    ///
    uint8_t     m_ledNum;
    ///
    Tls&        m_tlsKey;
    ///
    size_t      m_tlsCounter;

public:
    ///
    MyRunnable( Tls& tlsKey, uint8_t ledNum )
        :m_ledNum(ledNum),
         m_tlsKey(tlsKey),
         m_tlsCounter(ledNum)
            {
            }

public:
    ///
    void appRun()
        {
        if ( m_tlsKey.get() != 0 )
            {
            FatalError::logf( "(%s) Bad initial TLS value (%p)", Thread::myName(), m_tlsKey.get() );
            }

        for(;;)
            {
            m_tlsKey.set( (void*) m_tlsCounter );
            toggleLED();
            //Thread::wait();
            //toggleLED();
            //Thread::wait();
            if ( m_tlsKey.get() != (void*) m_tlsCounter )
                {
                FatalError::logf( "(%s) Bad TLS value (%p) - should be (%p)", Thread::myName(), m_tlsKey.get(), m_tlsCounter );
                }
            m_tlsCounter++;
            }
        }


    void toggleLED()
        {
        if ( m_ledNum == 1 )
            {
            Bsp_Api_toggle_debug1();
            }
        else
            {
            Bsp_Api_toggle_debug2();
            }
        }
};

class MyRunnable2: public Runnable
{
public:
    ///
    Thread&                     m_ledThread;
    ///
    uint16_t                    m_onTime_ms;
    ///
    uint16_t                    m_offTime_ms;
    ///
    ElaspedTime::Precision_T    m_ptime;
    ///
    unsigned long               m_msec;

public:
    MyRunnable2( Thread& ledThread, uint16_t onTime_ms, uint16_t offTime_ms )
    :m_ledThread(m_ledThread),
     m_onTime_ms(onTime_ms),
     m_offTime_ms(offTime_ms)
        {
        }
                
public:
    void appRun()
        {
        unsigned long sleepTime = m_onTime_ms + m_offTime_ms;
        m_ptime                 = ElaspedTime::precision();
        m_msec                  = ElaspedTime::milliseconds();

        for(;;)
            {
            Api::sleep( m_onTime_ms );
            //m_ledThread.signal();
            Api::sleep( m_offTime_ms );
            //m_ledThread.signal();


            ElaspedTime::Precision_T ptime     = ElaspedTime::precision();
            unsigned long            msec      = ElaspedTime::milliseconds();
            unsigned long            deltaM    = ElaspedTime::deltaMilliseconds( m_msec, msec );
            ElaspedTime::Precision_T deltaP    = ElaspedTime::deltaPrecision( m_ptime, ptime );
            unsigned long            flatten   = deltaP.m_seconds * 1000 + deltaP.m_thousandths;
             
            if ( flatten < sleepTime )
                {
                FatalError::logf( "Elasped Precision_T delta is wrong" );
                }
             
            if ( deltaM < sleepTime )
                {
                FatalError::logf( "Elasped msec delta is wrong" );
                }

            if ( flatten < deltaM - 1 || flatten > deltaM + 1 )
                {
                FatalError::logf( "Precision time is not insync with milliseconds time" );
                }

            m_ptime = ptime;
            m_msec  = msec;
            }
        }
};


};  // end namespace


////////////////////////////////////////////////////////////////////////////////

void runtests( void )
    {
    // Create my TLS key (can't be done statically)
    //Tls* keyPtr = new Tls();
    Tls key;

    // Create some threads....
    MyRunnable  appleLed( key, 1 );
    Thread*     appledLedPtr = Thread::create( appleLed, "AppleLED" );
    MyRunnable2 appleTimer( *appledLedPtr, 1000, 1000 );
    Thread::create( appleTimer, "AppleTimer" );

//    MyRunnable orangeLed( key, 2 );
//    Thread* orangeLedPtr = Thread::create( orangeLed, "OrangeLED" );
//    MyRunnable2 orangeTimer( *orangeLedPtr, 1500, 250 );
//    Thread::create( orangeTimer, "OrangeTimer" );
  
    
    // Start the schedular
    Api::enableScheduling();
    }
