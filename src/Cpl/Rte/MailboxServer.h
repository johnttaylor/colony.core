#ifndef Cpl_Rte_MailboxServer_h_
#define Cpl_Rte_MailboxServer_h_
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

#include "Cpl/Itc/MailboxServer.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Container/DList.h"
#include "Cpl/Rte/Subscriber.h"


///
namespace Cpl {
///
namespace Rte {

/** This class extends the Cpl::Itc::MailboxServer class to support the
    asynchronous change notification generated from Model Points.

    The order of processing is:

       1. The timers and their callbacks (if any timers have expired) are
          processed.
       2. Event Flags are processed.  Events are processed in LSb order.
       3. A single Model Point Change notification (if there is one pending) is
          processed.
       4. A single ITC message (if one was received) is processed.
       5. The loop is repeated until there are no expired timers, no event
          flags, no MP change notifications, and no ITC messages - at which
          point the thread blocks and wait for any of the above asynchronous
          actions to wake up the thread.

 */
class MailboxServer : public Cpl::Itc::MailboxServer
{
protected:
    /// List of pending Model Point Change Notifications
    Cpl::Container::DList<Subscriber>   m_pendingMpNotifications;


public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Cpl::Timer::Local Timers.

        NOTE: A value of zero for 'timingTickInMsec' will disable support
              for Local Timers.
     */
    MailboxServer( unsigned long timingTickInMsec = OPTION_CPL_ITC_MAILBOX_TIMEOUT_PERIOD ) throw();


protected:
    /** This method is responsible for processing Model Point change
        notifications.  If the Application needs to extend this class to
        process Event Flags - it should override the appProcessEventFlag()
        method (which this method calls)
     */
    void processEventFlag( uint8_t eventNumber ) throw();

    /** This method is used by a child class to provide application
        processing of the Mailbox's Event Flags.  The default implementation
        does nothing.
     */
    virtual void appProcessEventFlag( uint8_t eventNumber ) throw();


public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is used add a new 'change notification' to its list
        of pending change notifications

        NOTE: If the Subscriber's mailbox does not match this instance a
              fatal error is generated.  The requirements/semantics of Model
              Point subscription is that Subscription/Notifications/Cancel-of-
              Subscriptions all happen in a SINGLE thread and that the
              Subscriber object has reference to that thread's mailbox server.
     */
    void addPendingChangingNotification_( Subscriber& subscriber ) throw();

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
    by other classes in the Cpl::Rte namespace.  The Application should
    NEVER call this method.

    This method is used remove a pending 'change notification' from its list
    of pending change notifications

    NOTE: If the Subscriber's mailbox does not match this instance a
          fatal error is generated.  The requirements/semantics of Model
          Point subscription is that Subscription/Notifications/Cancel-of-
          Subscriptions all happen in a SINGLE thread and that the
          Subscriber object has reference to that thread's mailbox server.
 */
    void removePendingChangingNotification_( Subscriber& subscriber ) throw();

};

};      // end namespaces
};
#endif  // end header latch