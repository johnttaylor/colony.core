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
#include "Cpl/Rte/SubscriberApi.h"
#include "Cpl/Rte/NotificationApi_.h"


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
       3. A single ITC message (if one was received) is processed.
       4. A single Model Point Change notification (if there is one pending) is
          processed.
       5. The loop is repeated until there are no expired timers, no event
          flags, no ITC messages, and no MP change notifications - at which
          point the thread blocks and wait for any of the above asynchronous
          actions to wake up the thread.

 */
class MailboxServer : public Cpl::Itc::MailboxServer, public NotificationApi_
{
protected:
    /// List of pending Model Point Change Notifications
    Cpl::Container::DList<SubscriberApi>   m_pendingMpNotifications;


public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Cpl::Timer::Local Timers.

        NOTE: A value of zero for 'timingTickInMsec' will disable support
              for Local Timers.
     */
    MailboxServer( unsigned long timingTickInMsec = OPTION_CPL_ITC_MAILBOX_TIMEOUT_PERIOD ) throw();


protected:
    /** This method is responsible for processing the Model Point change
        notifications event.  If the Application needs to extend this class to
        process Event Flags - it should override the appProcessEventFlag()
        method (which this method calls)
     */
    virtual void processEventFlag( uint8_t eventNumber ) throw();

    /** This method is used by a child class to provide application
        processing of the Mailbox's Event Flags.  The default implementation
        does nothing.
     */
    virtual void appProcessEventFlag( uint8_t eventNumber ) throw();

    /** See Cpl::Itc::MailboxServer.  Note: This method performs the actual
        Model Point change notifications.
     */
    void endOfLoopProcessing() throw();

    /// See Cpl::Itc::Mailbox
    bool isPendingActions() throw();


public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is used add a new 'change notification' to its list
        of pending change notifications.  Calling this method when the
        subscriber is already registered for change notification will cause
        a FATAL ERROR.

        This method IS thread safe.

        NOTE: The requirements and/or semantics of Model Point subscription is
              that Subscriptions, Notifications, and Cancel-of-Subscriptions
              all happen in a SINGLE thread and that thread is the 'Subscribers'
              thread.
     */
    void addPendingChangingNotification_( SubscriberApi& subscriber ) throw();

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
    by other classes in the Cpl::Rte namespace.  The Application should
    NEVER call this method.

    This method is used remove a pending 'change notification' from its list
    of pending change notifications.  It is okay to call this method even if
    the Subscriber is not current registered for change notifications.

     This method IS thread safe.

     NOTE: The requirements and/or semantics of Model Point subscription is
           that Subscriptions, Notifications, and Cancel-of-Subscriptions
           all happen in a SINGLE thread and that thread is the 'Subscribers'
           thread.
     */
    void removePendingChangingNotification_( SubscriberApi& subscriber ) throw();

};

};      // end namespaces
};
#endif  // end header latch