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

#include "Mailbox.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/SimTick.h"


///
using namespace Cpl::Itc;


////////////////////////////////////////////////////////////////////////////////
Mailbox::Mailbox( unsigned long timeOutPeriodInMsec )
    :m_timeout( timeOutPeriodInMsec )
    , m_waiting( false )
    , m_events( 0 )
{
}


void Mailbox::post( Message& msg ) throw()
{
    // Update my internal FIFO
    m_flock.lock();
    put( msg );
    bool waiting = m_waiting;
    if ( waiting )
    {
        m_waiting = false;
    }
    m_flock.unlock();

    // Only signal myself if the mailbox is current blocked waiting for a next message
    if ( waiting )
    {
        m_sema.signal();
    }
}

void Mailbox::postSync( Message& msg ) throw()
{
    post( msg );
    CPL_SYSTEM_SIM_TICK_APPLICATION_WAIT();
    Cpl::System::Thread::wait();
}

bool Mailbox::isPendingActions() throw()
{
    return false;
}

Message* Mailbox::waitNext( bool& wasTimeout, Cpl_Itc_EventFlags_T& eventFlags ) throw()
{
    Message* msgPtr = 0;
    wasTimeout      = false;
    eventFlags      = 0;

    for ( ;;)
    {
        // Get the indicator for any pending action(s).
        // NOTE: This must be DONE OUTSIDE of the fast-lock since the parent
        //       class cannot enforce the semantics (e.g. non-recursive) of 
        //       the FastLock on the child class(es)
        bool pendingActions = isPendingActions();

        // START critical section
        m_flock.lock();

        // Get the next message
        msgPtr = get();

        // EXIT the wait loop IF I received message OR an Event Flag was set OR a child implementation has at least on pending action
        if ( msgPtr || m_events || pendingActions )
        {
            eventFlags = m_events;
            m_events   = 0;
            m_flock.unlock();       // END the critical section
            break;
        }

        // Remember that I am waiting for a next message
        m_waiting = true;
        m_flock.unlock();           // END critical section


        // Wait for the next message
        if ( m_timeout )
        {
            // Note: For Simulation: the timedWait() call if it blocks - does so waiting on the next tick - so no Application Wait() call is needed    
            if ( !m_sema.timedWait( m_timeout ) )
            {
                // Change my waiting status to: NOT waiting
                m_flock.lock();
                m_waiting  = false;
                eventFlags = m_events;
                m_events   = 0;
                m_flock.unlock();

                wasTimeout = true;
                return 0;
            }
        }
        else
        {
            // For Simulation: Since this call can block 'past a tick boundary', I need a call Application Wait()
            CPL_SYSTEM_SIM_TICK_APPLICATION_WAIT();
            m_sema.wait();
        }
    }

    // Return the message found
    return msgPtr;
}


////////////////////////////////////////////////////////////////////////////////
void Mailbox::internalNotify_( Cpl_Itc_EventFlags_T events ) throw()
{
    // Mark that I was signaled and capture my 'waiting-on-message' state
    m_flock.lock();
    m_events    |= events;
    bool waiting = m_waiting;
    if ( waiting )
    {
        m_waiting = false;
    }
    m_flock.unlock();

    // Wake myself up (if I was waiting for a next-message)
    if ( waiting )
    {
        m_sema.signal();
    }
}


// NOTE: Same logic as internalNotify_(), EXCEPT no critical section is used -->this is because su_signal() is called from an ISR and no mutex is required (and mutexes don't work in from ISRs anyway)
void Mailbox::su_internalNotify_( Cpl_Itc_EventFlags_T events ) throw()
{
    // Mark that I was signaled and capture my 'waiting-on-message' state
    m_events    |= events;
    bool waiting = m_waiting;
    if ( waiting )
    {
        m_waiting = false;
    }

    // Wake myself up (if I was waiting for a next-message)
    if ( waiting )
    {
        m_sema.su_signal();
    }
}


