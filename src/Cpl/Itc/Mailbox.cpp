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

#include "Mailbox.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/SimTick.h"


///
using namespace Cpl::Itc;

/** NOTE to self so I don't forget the implementation logic.

    The post(), signal(), and suSignal() ONLY invoke the mailbox's
    _waiters' signal method if the mailbox is in the waiting-state
    (a.k.a. _waiter != 0).  The reason behind this is because the
    mailbox is using the builtin semaphores of the individual
    threads for it blocking/wake-up functionality.  However, the
    thread semaphore is also used to implement synchronous
    ITC (and possible other application uses).  Since the use
    of the thread semaphore serves multiple uses, the mailbox
    logic insures that the 'wake-up' action associated with
    the mailbox only signals the thread when the thread is
    ACTUALLY waiting on the mailbox.  
**/

Mailbox::Mailbox( unsigned long timeOutPeriodInMsec )
:m_timeout(timeOutPeriodInMsec)
,m_waiting(false)
,m_signaled(false)
    {
    }


void Mailbox::post(Message& msg) throw() 
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

void Mailbox::postSync(Message& msg) throw() 
    {
    post( msg );
    CPL_SYSTEM_SIM_TICK_APPLICATION_WAIT();
    Cpl::System::Thread::wait();
    }

Message* Mailbox::waitNext( bool& wasTimeout ) throw() 
    {
    Message* msgPtr;
    wasTimeout = false;

    for(;;)
        {
        // START critcal section
        m_flock.lock();
        
        // Trap if I was signalled
        if ( m_signaled )
            {
            m_signaled = false;
            m_flock.unlock();       // END the critical section
            return 0;
            }

        // Get the next message
        msgPtr = get();
        if ( msgPtr )
            {
            m_flock.unlock();       // END the critical section
            break;
            }
        
        // Remember that I am waiting for a next message
        m_waiting = true;
        m_flock.unlock();           // END critcal section


        // Wait for the next message
        if ( m_timeout )
            {
            // Note: For Simulation: the timedWait() call if it blocks - does so waiting on the next tick - so no Application Wait() call is needed    
            if ( !m_sema.timedWait( m_timeout ) )
                {
                // Change my waiting status to: NOT waiting
                m_flock.lock();
                m_waiting = false;
                m_flock.unlock();          

                wasTimeout = true;
                return 0;
                }
            }
        else
            {
            // For Simulation: Since this call can block 'past a tick boundary', I need a call Appliation Wait()
            CPL_SYSTEM_SIM_TICK_APPLICATION_WAIT();
            m_sema.wait();
            }
        }

    // Return the message found
    return msgPtr;
    }


int Mailbox::signal(void) throw() 
    {
    int result = 0;

    // Mark that I was signaled and capture my 'waiting-on-message' state
    m_flock.lock();
    m_signaled   = true;
    bool waiting = m_waiting;
    if ( waiting )
        {
        m_waiting = false;
        }
    m_flock.unlock();

    // Wake myself up (if I was waiting for a next-message)
    if( waiting ) 
        {
        result = m_sema.signal();
        }

    return result;
    }


// NOTE: Same logic as signal(), EXCEPT no critical section is used -->this is because su_signal() is called from an ISR and no mutex is required (and mutexes don't work in from ISRs anyway)
int Mailbox::su_signal(void) throw() 
    {
    int result = 0;

    // Mark that I was signaled and capture my 'waiting-on-message' state
    m_signaled   = true;
    bool waiting = m_waiting;
    if( waiting )
        {
        m_waiting = false;
        }

    // Wake myself up (if I was waiting for a next-message)
    if( waiting ) 
        {
        result = m_sema.su_signal();
        }

    return result;
    }
