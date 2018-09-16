#ifndef Cpl_Rte_SubscriberComposer_h_
#define Cpl_Rte_SubscriberComposer_h_
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

#include "Cpl/Rte/SubscriberApi.h"

///
namespace Cpl {
///
namespace Rte {


/** This template class is a composer pattern/class that manages the callback
    function for a Model Point's Subscribers/Observers change notification

    A Composer is a structural pattern that may be used to employ composition
    when implementing an interface rather than using multiple inheritance. This
    allows a single concrete object to receive the RMW callbacks from many
    Model Points.

    Template Arguments:
        CONTEXT - The class that implements the Callback function
        MP       - The concrete Model Point type.
 */
template <class CONTEXT, class MP>
class SubscriberComposer : public Subscriber<MP>
{
public:
    /** Define a callback method function for the Change Notification callback (See
        Cpl::Rte::Subscriber<MP>::modelPointChanged for additional details)
     */
    typedef void (CONTEXT::*NotificationFunc_T)(MP& modelPointThatChanged);


protected:
    /// Class the implement the callback
    CONTEXT&                    m_context;

    /// Method (in my Context) to call for the change notification
    NotificationFunc_T          m_notificationCb;


public:
    /// Constructor
    SubscriberComposer( Cpl::Rte::MailboxServer& myMailbox,
                        CONTEXT&                 context,
                        NotificationFunc_T       notifyCallback );


public:
    /// See Cpl::Rte::Subscriber<MP>
    void modelPointChanged( MP& modelPointThatChanged ) throw();

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <class CONTEXT, class MP>
Cpl::Rte::SubscriberComposer<CONTEXT, MP>::SubscriberComposer( Cpl::Rte::MailboxServer& myMailbox,
                                                               CONTEXT&                 context,
                                                               NotificationFunc_T       notifyCallback )
    :Subscriber<MP>( myMailbox )
    , m_context( context )
    , m_notificationCb( notifyCallback )
{
}

/////////////////
template <class CONTEXT, class MP>
void Cpl::Rte::SubscriberComposer<CONTEXT, MP>::modelPointChanged( MP& modelPointThatChanged ) throw()
{
    // Notify context
    return (m_context.*m_notificationCb)(modelPointThatChanged);
}


};      // end namespaces
};
#endif  // end header latch
