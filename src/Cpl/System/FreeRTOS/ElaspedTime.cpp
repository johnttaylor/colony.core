/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Cpl/System/ElaspedTime.h"
#include "FreeRTOS.h"
#include "task.h"


/// 
using namespace Cpl::System;

extern "C" void vApplicationTickHook( void );


///////////////////////////////////////////////////////////////
static uint16_t      secondsDivider_;
static unsigned long elaspedSeconds_;

///////////////////////////////////////////////////////////////
void vApplicationTickHook( void )
    {
    if ( ++secondsDivider_ >= 1000 / portTICK_PERIOD_MS )
        {
        secondsDivider_ = 0;
        elaspedSeconds_++;
        }
    }


///////////////////////////////////////////////////////////////
unsigned long ElaspedTime::milliseconds( void ) throw()
    {
    return millisecondsInRealTime();
    }


unsigned long ElaspedTime::seconds( void ) throw()
    {
    return secondsInRealTime();
    }


ElaspedTime::Precision_T ElaspedTime::precision( void ) throw()
    {
    return precisionInRealTime();
    }


///////////////////////////////////////////////////////////////
unsigned long ElaspedTime::millisecondsInRealTime( void ) throw()
    {
    unsigned long s;
    uint16_t      ms;

    // Note: I use my own counter because it was the only way to guaranty that milliseoncds and seconds are in sync
    taskDISABLE_INTERRUPTS();
    s  = elaspedSeconds_;
    ms = secondsDivider_;
    taskENABLE_INTERRUPTS();

    return (ms * portTICK_PERIOD_MS) + s * 1000L;
    }

unsigned long ElaspedTime::secondsInRealTime( void ) throw()
    {
    unsigned long s;

    taskDISABLE_INTERRUPTS();
    s  = elaspedSeconds_;
    taskENABLE_INTERRUPTS();

    return s;
    }


ElaspedTime::Precision_T ElaspedTime::precisionInRealTime( void ) throw()
    {
    ElaspedTime::Precision_T now;

    taskDISABLE_INTERRUPTS();
    now.m_seconds     = elaspedSeconds_;
    now.m_thousandths = secondsDivider_;
    taskENABLE_INTERRUPTS();

    now.m_thousandths *= portTICK_PERIOD_MS;
    return now;
    }
