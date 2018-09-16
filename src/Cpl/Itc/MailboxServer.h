#ifndef Cpl_Itc_MailboxServer_h_
#define Cpl_Itc_MailboxServer_h_
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
#include "Cpl/Itc/Mailbox.h"
#include "Cpl/Timer/CounterList.h"
#include "Cpl/Timer/CounterList.h"


///
namespace Cpl {
///
namespace Itc {

/** This class collects the common functions of a generic server providing a
    mailbox and a main control loop.

    The mailbox server is further augmented with the support for Local
    Timers (see Cpl::Timer::Local).  A local timer is started, stopped,
    and expires all within the context of the thread that the mailbox
    server is associated with.  When the timer expires, it will generate
    a callback to its CONTEXT instance.  The order of processing is:

       1. The timers and their callbacks (if any timers have expired) are
          processed.
       2. Event Flags are processed.  Events are processed in LSb order.
       3. A Single ITC message (if one was received) is processed.
       4. The loop is repeated until there are no expired timers, no event
          flags, no ITC messages - at which point the thread blocks and wait
          for any of the above asynchronous actions to wake up the thread.

    The timing source for mailbox server is based on timed-wait operations
    of a semaphore and should not be considered a deterministic timing
    source.  In addition, the execution of the callback of a expired timer
    is subject to the processing that is ongoing in the thread, i.e. if a
    message takes a relatively long time to be process, it can potentially
    delay the execution of the timer callback - thus making precision timing
    intervals even more variable.  However, the timer mechanism is guaranteed
    to NOT expire early with respect to the specified timer duration.

 */
class MailboxServer :
    public Mailbox,
    public Cpl::System::Runnable,
    public Cpl::Timer::CounterList
{
public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Cpl::Timer::Local Timers.

        NOTE: A value of zero for 'timingTickInMsec' will disable support
              for Local Timers.
     */
    MailboxServer( unsigned long timingTickInMsec = OPTION_CPL_ITC_MAILBOX_TIMEOUT_PERIOD ) throw();

public:
    /** This method is use to request the mailbox server to quit its processing
        of messages and to terminate the thread.
        NOTES:

            o This is ONLY a request and it WILL NOT brute force the thread to
              terminate. The request will ONLY succeed if the mailbox
              server/thread is blocked on waiting for a 'next-message' or
              attempts to process a 'next-message'.
            o Any message(s) remaining in the mailbox will NOT be processed.
            o This is call is asynchronous to the mailbox's execution, i.e.
              the method will return immediately, but when the mailbox actually
              terminates is not guaranteed.
            o This method is safe to be called ANY thread including from the
              Mailbox Server's thread.
     */
    void pleaseStop();


protected:
    /// See Cpl::System::Runnable
    void appRun();

protected:
    /** This initialization function is called before the server
        enters its main loop. The default implementation does nothing.
        Sub-classes may override this function to perform specific run-time
        initialization.
     */
    virtual void initialize() throw();

    /** This cleanup function is called when server EXITS its
        main loop, i.e. when the thread is self terminating. The default
        implementation does nothing. Sub-classes may override this function
        to perform specific run-time clean-up on thread termination.  NOTE:
        this method is ALWAYS run when the mailbox server/thread terminates,
        i.e. even when pleaseStop() is called.
     */
    virtual void cleanup() throw();

    /** This method is used (by the concrete child class(es)) to process one
        or more Event Flags.  This method is called when the mailbox is
        un-blocked and at least Event Flag is set.  The method is called N
        consecutive times - one call for each Event Flag that is set.  The
        'eventNumber' (which is zero based) identifies which Event Flag is/was
        set.

        This method is NOT called for the reserved/internal Event Flag(s).

        The default implementation of this method does NOTHING.
     */
    virtual void processEventFlag( uint8_t eventNumber ) throw();

    /** This method provide a hook for the child classes to do additional
        processing and the 'end' of the loop processing, i.e. this method is
        called after the ITC message processing.

        The default implementation of this method does NOTHING.
     */
    virtual void endOfLoopProcessing() throw();

protected:
    /// See Cpl::Timer::CounterSource
    unsigned long msecToCounts( unsigned long durationInMsecs ) throw();

private:
    /** NOTE This a 'private' method see Cpl::System::Runnable for more 
        details.
     */
    void setThreadOfExecution_( Cpl::System::Thread* myThreadPtr );


protected:
    /// My thread pointer
    Cpl::System::Thread* m_myThreadPtr;

    /// Time of the current tick cycle
    unsigned long m_timeNow;

    /// Flag used to help with the pleaseStop() request
    bool m_run;

};

};      // end namespaces
};
#endif  // end header latch