#ifndef Cpl_System_Framework_h_
#define Cpl_System_Framework_h_
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
/** @file */

#include "Cpl/System/Runnable.h"
#include "Cpl/System/FastLock.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/System/EventFlag.h"


/** Specifies the default timeout period for waiting on a event.
 */
#ifndef OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD
#define OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD       1  /// 1 msec timeout, aka 1 msec timer resolution for Software Timers
#endif

 ///
namespace Cpl {
///
namespace System {


/** This concrete class is a Runnable object that provides a event driven
    execution model for a thread.  The thread will remaining blocked until
    an "event" occurs.  The following "events" are supported:

    1) The EventLoop Semaphore was signaled
    2) A Software Timer expiring (Cpl::System::Timer)
    3) A Event Flag was signaled( Cpl::System::EventFlag)

    The class also provides a mechanism for registering 'other' events to
    be proccessed.

    Note: The EventLoop does NOT use/consume the Thread Semaphore.
 */



 /** This is an abstract class defines the interface for an object
     that is "executed" when a Thread object is created.
  */
class EventLoop : public Runnable, public EventFlag
{
public:
    /** Constructor. The 'timeOutPeriodInMsec' parameter specifies how
        long the EventLoop will wait for an event before returning
        from the waitNext() call.  If 'timeOutPeriodInMsec' is zero, then
        the EventLoop will wait forever, i.e. never timeout.  The value of
        'timeOutPeriodInMsec' is resolution of the timing source for the
        Software Timers.
     */
    EventLoop( unsigned long timeOutPeriodInMsec=OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD );

    /// Virtual destructor
    virtual ~EventLoop() {};

public:
    /** This method used to signal/wake-up the EventLoop
     */
    virtual void wakeUp() throw();

public:
    /// See Cpl::System::Runnable
    void pleaseStop();


protected:
    /// See Cpl::System::Runnable
    void appRun();

public:
    /// See Cpl::System::EventFlag
    void notifyEvents( Cpl_System_EventFlag_T events ) throw();

    /// See Cpl::System::EventFlag
    void notify( uint8_t eventNumber ) throw();

    /// See Cpl::System::EventFlag
    void su_notifyEvents( Cpl_System_EventFlag_T events ) throw();

    /// See Cpl::System::EventFlag
    void su_notify( uint8_t eventNumber ) throw();


protected:
    /// My thread pointer
    Cpl::System::Thread* m_myThreadPtr;

    /// Time of the current tick cycle
    unsigned long           m_timeNow;

    /// Flag used to help with the pleaseStop() request
    bool                    m_run;

    /// Mutex used to protect my list
    Cpl::System::FastLock   m_flock;

    /// Semaphore associated with the mailbox (note: the Thread semaphore is NOT used)
    Cpl::System::Semaphore  m_sema;

    /// Timeout period for waiting on the next message
    unsigned long           m_timeout;

    /// The variable holds the current state of all Event Flags
    Cpl_System_EventFlag_T  m_events;

};

};      // end namespaces
};
#endif  // end header latch
