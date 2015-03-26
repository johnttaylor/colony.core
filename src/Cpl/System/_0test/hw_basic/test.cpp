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
//#include "Cpl/System/Semaphore.h"
//#include "Cpl/System/ElaspedTime.h"
//#include "Cpl/System/Tls.h"



#define SECT_     "_0test"

/// 
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////
namespace {

class MyRunnable: public Runnable
{
public:
    ///
    uint16_t    m_onTime_ms;
    ///
    uint16_t    m_offTime_ms;
    ///
    uint8_t     m_ledNum;

public:
    ///
    MyRunnable( uint16_t ontime_ms, uint16_t offtime_ms, uint8_t ledNum )
        :m_onTime_ms(ontime_ms),
         m_offTime_ms(offtime_ms),
         m_ledNum(ledNum)
            {
            }

public:
    ///
    void appRun()
        {
        for(;;)
            {
            toggleLED();
            Api::sleep( m_onTime_ms );
            toggleLED();
            Api::sleep( m_offTime_ms );
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
};


////////////////////////////////////////////////////////////////////////////////
static MyRunnable appleRun_( 1000, 1000, 1 );
static MyRunnable orangeRun_( 250, 1000, 2 );

void runtests( void )
    {
    // Create some threads....
    Thread::create( appleRun_, "Apple" );

    Thread::create( orangeRun_, "Orange" );
    
    // Start the schedular
    Api::enableScheduling();
    }
