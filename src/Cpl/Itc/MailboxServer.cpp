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

#include "MailboxServer.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Trace.h"


#define SECT_ "Cpl::Itc"


///
using namespace Cpl::Itc;

/////////////////////
MailboxServer::MailboxServer( unsigned long timingTickInMsec ) throw()
:Mailbox( timingTickInMsec ),
 m_timeNow( 0 ),
 m_run(true)
    {
    }


/////////////////////
void MailboxServer::appRun()
    {
    // Application hook
    initialize();

    // Get the initial starting time
    unsigned long timeMark = Cpl::System::ElapsedTime::milliseconds();

    // Process messages forever (or until told to stop)
    for(;;)
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

        // Wait/get the next-message to process
        bool wasTimeout = false;
        Message* msgPtr = waitNext( wasTimeout );

        // Trap my exit/please-stop condition AGAIN since alot could have happen while I was waiting....
        m_flock.lock();
        stayRunning = m_run;
        m_flock.unlock();
        if ( !stayRunning )
            {
            break;
            }

        // Process Local Timers
        if ( m_timeout )
            {
            // Calculate the elapsed time in milliseconds
            m_timeNow               = Cpl::System::ElapsedTime::milliseconds();
            unsigned long deltaTime = Cpl::System::ElapsedTime::deltaMilliseconds( timeMark, m_timeNow );

            // Update my timers
            CPL_SYSTEM_TRACE_MSG( SECT_, (" @@ START TICK: %lu, now=%lu, [timeMark=%lu]", deltaTime, m_timeNow, timeMark ));
            tick( deltaTime );
            timeMark = m_timeNow;
            CPL_SYSTEM_TRACE_MSG( SECT_, (" @@ TICK COMPLETE..., timeMark=%lu", timeMark ));
            tickComplete();
            }

        // No next-message 
        if ( !msgPtr )
            {
            // The mailbox was signaled
            if ( !wasTimeout )
                {
                signaled();
                }
            }

        // Process the next-message
        else 
            {
            msgPtr->process();
            }
        }

    // Application hook
    cleanup();
    }

unsigned long MailboxServer::msecToCounts( unsigned long durationInMsecs ) throw()
    {
    unsigned long delta = Cpl::System::ElapsedTime::deltaMilliseconds( m_timeNow );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("duration IN=%lu, count out=%lu",durationInMsecs, durationInMsecs +  delta ));
    return durationInMsecs + delta;
    }

/////////////////////
void MailboxServer::pleaseStop()
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    // Set my flag/state to exit my top level thread loop
    m_flock.lock();
    m_run = false;
    m_flock.unlock();

    // Signal myself in case the thread is blocked waiting for the 'next message'
    m_sema.signal();
    }


/////////////////////
void MailboxServer::initialize() throw()
    {
    }

void MailboxServer::cleanup() throw()
    {
    }

void MailboxServer::signaled() throw()
    {
    // Default is to do: NOTHING
    }