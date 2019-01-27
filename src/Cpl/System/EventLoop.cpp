/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "EventLoop.h"
#include "SimTick.h"
#include "Trace.h"
#include "FatalError.h"

#define SECT_ "Cpl::System"


///
using namespace Cpl::System;

/////////////////////
EventLoop::EventLoop( unsigned long      timeOutPeriodInMsec,
                      EventLoopCallback* extraEventProcessor1,
                      EventLoopCallback* extraEventProcessor2,
                      EventLoopCallback* extraEventProcessor3 )
    : m_flock()
    , m_sema()
    , m_processor1( extraEventProcessor1 )
    , m_processor2( extraEventProcessor2 )
    , m_processor3( extraEventProcessor3 )
    , m_timeout( timeOutPeriodInMsec )
    , m_events( 0 )
    , m_run( true )
{
    if ( timeOutPeriodInMsec == 0 )
    {
        FatalError::logf( "EventLoop(%p): timeOutPeriodInMsec can NOT be set to zero", this );
    }
}

int EventLoop::signal( void ) throw()
{
    return m_sema.signal();
}

int EventLoop::su_signal( void ) throw()
{
    return m_sema.su_signal();
}

void EventLoop::appRun( void )
{
    // Initialize/start the timer manager
    startManager();

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
        m_sema.timedWait( m_timeout ); // Note: For Simulation: the timedWait() call if it blocks - does so waiting on the next tick - so no Application Wait() call is needed

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
        Cpl_System_EventFlag_T events = m_events;
        m_events                      = 0;
        m_flock.unlock();

        // Process Event Flags
        if ( events )
        {
            Cpl_System_EventFlag_T eventMask   = 1;
            uint8_t                eventNumber = 0;
            for ( ; eventMask; eventMask <<= 1, eventNumber++ )
            {
                if ( (events & eventMask) )
                {
                    processEventFlag( eventNumber );
                }
            }
        }

        // Timer Check
        processTimers();

        // Hooks for additional processing
        if ( m_processor1 )
        {
            m_processor1->processCustomEvent( m_timeNow );
        }
        if ( m_processor2 )
        {
            m_processor2->processCustomEvent( m_timeNow );
        }
        if ( m_processor3 )
        {
            m_processor3->processCustomEvent( m_timeNow );
        }
    }
}

/////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void EventLoop::notifyEvents( Cpl_System_EventFlag_T events ) throw()
{
    // Mark that I was signaled
    m_flock.lock();
    m_events |= events;
    m_flock.unlock();

    m_sema.signal();
}

void EventLoop::notify( uint8_t eventNumber ) throw()
{
    notifyEvents( 1 << eventNumber );
}

// NOTE: Same logic as notifyEvents(), EXCEPT no critical section is used -->this is because su_signal() is called from an ISR and no mutex is required (and mutexes don't work in from ISRs anyway)
void EventLoop::su_notifyEvents( Cpl_System_EventFlag_T events ) throw()
{
    // Mark that I was signaled 
    m_events    |= events;
    m_sema.su_signal();
}

void EventLoop::su_notify( uint8_t eventNumber ) throw()
{
    su_notifyEvents( 1 << eventNumber );
}

