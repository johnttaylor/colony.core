#ifndef Cpl_Rte_Subscriber_h_
#define Cpl_Rte_Subscriber_h_
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


#include "Cpl/Rte/Point.h"
#include "Cpl/Container/Item.h"

///
namespace Cpl {
///
namespace Rte {


/** This partially concrete class defines the Subscriber - for change 
    notifications - interface to a Model Points data/state
 */
class SubscriberApi: public Cpl::Container::Item
{
public:
    /// This method notifies the client that MP was changed
    virtual void modelPointChanged( Point& pointThatChanged ) throw() = 0;

    // TODO: add data members to support the notification

public:
    /// Virtual destructor
    virtual ~SubscriberApi() {}
};

/** This template class implements the SubscriberApi interface that is context
    independent and allows for a single context to contain many Subscriber
    callbacks.

    The change notification's executes in the thread that was running when
    the application subscribed for the changed notification.  The callback is
    performed as part of the Rte::MailboxServer asynchronous notifications.

    Template args:
        CONTEXT   Type of the Class that implements the context for the Subscriber
  */
template <class CONTEXT>
class Subscriber : public SubscriberApi
{
public:
    /// Definition of the context/client call-back method 
    typedef void (CONTEXT::* ModelPointChangedFunction_T)(Point& pointThatChanged) throw();

private:
    ///
    ModelPointChangedFunction_T  m_callback;
    ///
    CONTEXT&                     m_context;


public:
    /// Constructor
    Subscriber( CONTEXT& contextInstance, ModelPointChangedFunction_T mpChangedCallbackFunc )
        :m_callback( mpChangedCallbackFunc ), m_context( contextInstance ) {}


private: // SubscriberApi
    ///
    void modelPointChanged( Point& pointThatChanged ) throw()
    {
        (m_context.*m_callback)(pointThatChanged);
    }
};


};      // end namespaces
};
#endif  // end header latch
