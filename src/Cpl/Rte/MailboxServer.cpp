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

#include "MailboxServer.h"
#include "SubscriberApi.h"
#include "ModelPoint.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"


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
    // Pass through all events EXCEPT for the Model Point Change Notification event
    // NOTE: The fact that the thread was woken-up is sufficient for the MP Change
    //       notification mechanism to work (i.e. no action needed here for
    //       the actual MP Change Notification event).
    if ( eventNumber != MP_EVENT_BIT_NUM )
    {
        appProcessEventFlag( eventNumber );
    }
}

bool MailboxServer::isPendingActions() throw()
{
    m_flock.lock();
    void* item = m_pendingMpNotifications.first();
    m_flock.unlock();
    return item != 0;

}

void MailboxServer::endOfLoopProcessing() throw()
{
    // Get the next pending change notification
    m_flock.lock();
    SubscriberApi* subscriberPtr = m_pendingMpNotifications.get();
    m_flock.unlock();

    // Execute - at MOST one - the change notification callback
    if ( subscriberPtr )
    {
        // Get the model point that changed
        ModelPoint* mpPtr = subscriberPtr->getModelPoint_();
        CPL_SYSTEM_ASSERT( mpPtr != 0 );   // NOTE: getModelPoint_() is guaranteed to return a valid pointer, but just in case...
        ModelPoint& modelPoint = *mpPtr; 

        // Update the subscriber's state
        modelPoint.processSubscriptionEvent_( *subscriberPtr, ModelPoint::eNOTIFYING );

        // Execute the callback
        subscriberPtr->genericModelPointChanged_( modelPoint );

        // Update the subscriber's state
        modelPoint.processSubscriptionEvent_( *subscriberPtr, ModelPoint::eNOTIFY_COMPLETE );
    }
}

void MailboxServer::addPendingChangingNotification_( SubscriberApi& subscriber ) throw()
{
    // Add the notification to my list and send myself an Event to wake up the mailbox
    m_flock.lock();
    m_pendingMpNotifications.put( subscriber );
    m_flock.unlock();

    internalNotify_( MP_EVENT_BIT_MASK );
}

void MailboxServer::removePendingChangingNotification_( SubscriberApi& subscriber ) throw()
{
    // Remove the subscriber from the notification
    m_flock.lock();
    m_pendingMpNotifications.remove( subscriber );
    m_flock.unlock();
}

void MailboxServer::appProcessEventFlag( uint8_t eventNumber ) throw()
{
    // Default is to do: NOTHING
}

