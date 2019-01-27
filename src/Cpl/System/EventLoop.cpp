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

#include "EventLoop.h"
#include "SimTick.h"
#include "Trace.h"

#define SECT_ "Cpl::System"


///
using namespace Cpl::System;

/////////////////////
EventLoop::EventLoop( unsigned long timeOutPeriodInMsec ) throw()
    :Runnable()
    , m_myThreadPtr( 0 )
    , m_timeNow( 0 )
    , m_run( true )
    , m_timeout( timeOutPeriodInMsec )
    , m_events( 0 )
{
}


void EventLoop::appRun( void )
{
    // Process events forever (or until told to stop)
    for ( ;;)
    {
        // Support simulated ticks
        CPL_SYSTEM_SIM_TICK_TOP_LEVEL_WAIT();

        // Trap my exit/please-stop condition
        m_flock.lock();
        bool stayRunning = m_run;
        m_flock.unlock();
        if ( !stayRunning )
        {
            break;
        }

        // Wait for something to happen...
        bool                   wasTimeout;
        Cpl_System_EventFlag_T events;
        if ( m_timeout )
        {
            // Note: For Simulation: the timedWait() call if it blocks - does so waiting on the next tick - so no Application Wait() call is needed
            if ( !m_sema.timedWait( m_timeout ) )
            {
                wasTimeout = true;
            }
        }
        else
        {
            // For Simulation: Since this call can block 'past a tick boundary', I need a call Application Wait()
            CPL_SYSTEM_SIM_TICK_APPLICATION_WAIT();
            m_sema.wait();
        }

        // Trap my exit/please-stop condition AGAIN since a lot could have happen while I was waiting....
        m_flock.lock();
        stayRunning = m_run;
        m_flock.unlock();
        if ( !stayRunning )
        {
            break;
        }

        // Capture the current state of the event flags
        m_flock.lock();
        events   = m_events;
        m_events = 0;
        m_flock.unlock();

        // TODO: Timer Check
        // TODO: Event processing
        // TODO: Hook for additional processing
    }
}

/////////////////////
void EventLoop::setThreadOfExecution_( Cpl::System::Thread* myThreadPtr )
{
    m_myThreadPtr = myThreadPtr;
}

void EventLoop::pleaseStop()
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    // Set my flag/state to exit my top level thread loop
    m_flock.lock();
    m_run = false;
    m_flock.unlock();

    // Signal myself in case the thread is blocked waiting for the 'next event'
    m_sema.signal();
}
