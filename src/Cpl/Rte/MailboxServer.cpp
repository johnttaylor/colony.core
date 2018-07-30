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
#include "Cpl/System/Trace.h"


#define SECT_ "Cpl::Rte"

#define MP_EVENT_BIT_NUM        ((Cpl_Itc_EventFlags_T)((sizeof(Cpl_Itc_EventFlags_T)*8)-1))
#define MP_EVENT_BIT_MASK       ((Cpl_Itc_EventFlags_T)(1<<MP_EVENT_BIT_NUM))


///
using namespace Cpl::Rte;

/////////////////////
MailboxServer::MailboxServer( unsigned long timingTickInMsec ) throw()
    :Cpl::Itc::MailboxServer( timingTickInMsec )
{
}


/////////////////////
void MailboxServer::processEventFlag( uint8_t eventNumber ) throw()
{
    if ( eventNumber != MP_EVENT_BIT_NUM )
    {
        appProcessEventFlag( eventNumber );
    }
    else
    {
        // process change notification
    }
}

void MailboxServer::addPendingChangingNotification_( Subscriber& subscriber ) throw()
{
    // Check for invalid Subscription semantics
    if ( subscriber.getMailbox_() != this )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::MailboxServer::addPendingChangingNotification(). Protocol Error. Adding a MP Subscriber (%p) that was configure to use a different MailboxServer (%p)", subscriber.getMailbox_(), this );
        return;
    }

    // Add the notification to my list and send myself an Event to wake up the mailbox
    m_pendingMpNotifications.put( subscriber );
    internalNotify_( MP_EVENT_BIT_MASK );
}

void MailboxServer::removePendingChangingNotification_( Subscriber& subscriber ) throw()
{
    // Check for invalid Subscription semantics
    if ( subscriber.getMailbox_() != this )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::MailboxServer::removePendingChangingNotification_(). Protocol Error. Removing a MP Subscriber (%p) that was configure to use a different MailboxServer (%p)", subscriber.getMailbox_(), this );
        return;
    }

    // Add the notification to my list and send myself an Event to wake up the mailbox
    m_pendingMpNotifications.remove( subscriber );
}

void MailboxServer::appProcessEventFlag( uint8_t eventNumber ) throw()
{
    // Default is to do: NOTHING
}

