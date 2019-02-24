#ifndef Cpl_Rte_Subscriber_h_
#define Cpl_Rte_Subscriber_h_
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
/** @file */


#include "Cpl/Rte/SubscriberApi.h"
#include "Cpl/Rte/ModelPoint.h"
#include "Cpl/Rte/EventLoop.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Rte {



/** This mostly concrete class defines the Subscriber interface - for change
    notifications - to a Model Points data/state
 */
class SubscriberBase : public SubscriberApi
{
protected:
    /// Internal state of the subscriber.  Note: The state is actual managed by the Model Point
    int                             m_state;

    /// Pointer to the Model Point the instance is subscribed to
    ModelPoint*                     m_point;

    /// Reference to subscriber's EventFlag/Mailbox server
    Cpl::Rte::EventLoop&            m_eventLoopHdl;

    /// Sequence number of the subscriber
    uint16_t                        m_seqNumber;

public:
    /// Constructor
    SubscriberBase( Cpl::Rte::EventLoop& myEventLoop );

public:
    /// See Cpl::Rte::SubscriberApi
    NotificationApi_* getNotificationApi_() const throw();

    /// See Cpl::Rte::SubscriberApi
    void setModelPoint_( ModelPoint* modelPoint ) throw();

    /// See Cpl::Rte::SubscriberApi
    ModelPoint* getModelPoint_() throw();

    /// See Cpl::Rte::SubscriberApi
    int getState_() const throw();

    /// See Cpl::Rte::SubscriberApi
    void setState_( int newState ) throw();

    /// See Cpl::Rte::SubscriberApi
    uint16_t getSequenceNumber_() const throw();

    /// See Cpl::Rte::SubscriberApi
    void setSequenceNumber_( uint16_t newSeqNumber ) throw();
};

/////////////////////////////////////////////////////////////////////////////

/** This template class defines a type safe Subscriber

    Template Arguments:
        MP      - The concrete Model Point Type
 */
template <class MP>
class Subscriber : public SubscriberBase
{
public:
    /// Type safe change notification.  See Cpl::Rte::SubscriberApi
    virtual void modelPointChanged( MP& modelPointThatChanged ) throw() = 0;

public:
    /// Constructor
    Subscriber( Cpl::Rte::EventLoop& myEventLoop ):SubscriberBase(myEventLoop) {}

protected:
    /// See Cpl::Rte::SubscriberApi
    void genericModelPointChanged_( ModelPoint& modelPointThatChanged ) throw() { modelPointChanged( *((MP*) &modelPointThatChanged) ); }
};



};      // end namespaces
};
#endif  // end header latch
