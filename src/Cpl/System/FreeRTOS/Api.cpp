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

#include "Cpl/System/Api.h"
#include "Cpl/System/Private_.h"
#include "Cpl/System/FatalError.h"
#include "Bsp/Api.h"
#include "FreeRTOS.h"
#include "task.h"


/// 
using namespace Cpl::System;

///
///
static Mutex systemMutex_;
static Mutex tracingMutex_;
static Mutex sysList_;


////////////////////////////////////////////////////////////////////////////////
void Api::initialize( void )
    {
    // Init the Colony.Core sub-systems
    StartupHook_::notifyStartupClients();
    }
    

void Api::enableScheduling( void )
    {
    // This method should never return
	vTaskStartScheduler();

    // If I get here something is wrong!!
    Bsp_Api_disableIrqs();
    for(;;)
        {
        Bsp_Api_nop();
        }
    }

void Api::sleep( unsigned long milliseconds ) throw()
    {
    vTaskDelay( milliseconds * portTICK_PERIOD_MS );
    }

void Api::sleepInRealTime( unsigned long milliseconds ) throw()
    {
    vTaskDelay( milliseconds * portTICK_PERIOD_MS );
    }




////////////////////////////////////////////////////////////////////////////////
Mutex& Locks_::system(void) 
    {
    return systemMutex_;
    }


Mutex& Locks_::tracing(void) 
    {
    return tracingMutex_;
    }

Mutex& Locks_::sysLists(void) 
    {
    return sysList_;
    }

