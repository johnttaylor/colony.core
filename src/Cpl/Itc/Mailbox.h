#ifndef Cpl_Itc_Mailbox_h_
#define Cpl_Itc_Mailbox_h_
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

#include "colony_config.h"
#include "Cpl/Itc/PostApi.h"
#include "Cpl/Itc/EventApi.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/System/FastLock.h"
#include "Cpl/Container/SList.h"


/** Specifies the default timeout period for waiting on message.
 */
#ifndef OPTION_CPL_ITC_MAILBOX_TIMEOUT_PERIOD
#define OPTION_CPL_ITC_MAILBOX_TIMEOUT_PERIOD       1  /// 1 msec timeout, aka 1 msec timer resolution for Local Timers
#endif


///
namespace Cpl {
///
namespace Itc {

/** This mailbox class implements an Inter Thread Communications mechanism
    based on message passing and Event Flags. There is no limit to the number
    of messages that can be stored in the queue at any given time since the
    FIFO queue and the messages uses the intrusive container mechanisms from
    the Cpl::Container namespace.

    The implementation provides support for implementing (by a child class) a
    time source for Local Timers (Cpl::Timer::Local)
 */

class Mailbox : public PostApi,
    public EventApi,
    public Cpl::Container::SList<Message>
{
public:
    /** Constructor. The 'timeOutPeriodInMsec' parameter specifies how
        long the mailbox will wait for message/Event Flag(s) before returning
        from the waitNext() call.  If 'timeOutPeriodInMsec' is zero, then
        the mailbox will wait forever, i.e. never timeout.  The value of
        'timeOutPeriodInMsec' is resolution of the timing source for Local
        Timers used by services that are associated with the mailbox.
     */
    Mailbox( unsigned long timeOutPeriodInMsec = OPTION_CPL_ITC_MAILBOX_TIMEOUT_PERIOD );


public:
    /** This operation is used by the owner thread of the mailbox
        to wait for messages to be posted by a client to the mailbox and/or for
        one or more Event Flags to be set. The calling thread blocks until a
        message is posted, an Event Flag is set, and/or the specified timeout
        period has expired. When this operation completes, either a pointer to
        a message, or zero is returned. A zero pointer indicates that there is
        message to process (i.e. unblocked due to Event Flags/timeout). The
        caller can use the 'wasTimeout' argument to differentiate between the
        mailbox being notify of Event Flag(s) being set and a timeout occurred.
        The 'eventFlags' argument returns which (if any) Event Flags were set
        when the method returns (a value of zero indicate NO Event Flags were
        set).

        Note: The current state of ALL Event Flags are cleared when this method
              returns.
     */
    virtual Message* waitNext( bool& wasTimeout, Cpl_Itc_EventFlags_T& eventFlags ) throw();


public:
    /// See Cpl::Itc::PostApi
    void post( Message& msg ) throw();

    /// See Cpl::Itc::PostApi
    void postSync( Message& msg ) throw();


public:
    /// Pull in methods from base class
    using Cpl::Itc::EventApi::notify;

    /// Pull in methods from base class
    using Cpl::Itc::EventApi::notifyEvents;

    /// See Cpl::Itc::EventApi
    void internalNotify_( Cpl_Itc_EventFlags_T events ) throw();

    /// See Cpl::Itc::EventApi
    void su_internalNotify_( Cpl_Itc_EventFlags_T events ) throw();


protected:
    /// Mutex used to protect my list
    Cpl::System::FastLock   m_flock;

    /// Semaphore associated with the mailbox (note: the Thread semaphore is used for managing synchronous ITC calls)
    Cpl::System::Semaphore  m_sema;

    /// Timeout period for waiting on the next message
    unsigned long           m_timeout;

    /// Keep track of when the mailbox is waiting for the next message
    bool                    m_waiting;

    /// The variable holds the current state of all Event Flags
    Cpl_Itc_EventFlags_T    m_events;

};


};      // end namespaces
};
#endif  // end header latch