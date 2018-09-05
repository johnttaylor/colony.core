#ifndef Cpl_Rte_SubscriberApi_h_
#define Cpl_Rte_SubscriberApi_h_
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

#include "Cpl/Container/Item.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Rte {

/// Forward reference to a Model point -->used to avoid circular dependencies
class ModelPoint;

/// Forward reference to the RTE Mailbox server -->used to avoid circular dependencies
class MailboxServer;


/** This mostly concrete class defines the Subscriber interface - for change
    notifications - to a Model Points data/state
 */
class SubscriberApi : public Cpl::Container::ExtendedItem
{
protected:
    /// Internal state of the subscriber.  Note: The state is actual managed by the Model Point
    int                 m_state;

    /// Pointer to the Model Point the instance is subscribed to
    ModelPoint*         m_point;

    /// Reference to subscriber's EventFlag/Mailbox server
    MailboxServer&      m_mailboxHdl;

    /// Sequence number of the subscriber
    uint16_t            m_seqNumber;

public:
    /// Constructor
    SubscriberApi( Cpl::Rte::MailboxServer& myMailbox );

protected:
    /** This method is the client's callback function for a MP change
        notification.  This method is called in as part of the asynchronous
        notification mechanism of the subscriber's Mailbox Server, i.e. executes 
        in the thread associated m_mailBoxHdl
     */
    virtual void genericModelPointChanged( ModelPoint& modelPointThatChanged ) throw() = 0;


public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method returns a pointer to the Subscriber's mailbox
      */
    inline Cpl::Rte::MailboxServer* getMailbox_() const throw()
    {
        return &m_mailboxHdl;
    }

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is use to set the Subscriber's Model Point reference
      */
    inline void setModelPoint_( ModelPoint* modelPoint ) throw()
    {
        m_point = modelPoint;
    }

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is use to get the Subscriber's Model Point reference.

        Note: If this method is called BEFORE the setModelPoint() method is
              called then a Fatal Error will be generated.
      */
    ModelPoint* getModelPoint_() throw();

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is use to get the Subscriber's internal state
      */
    inline int getState_() const throw()
    {
        return m_state;
    }

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is use to set the Subscriber's internal state
      */
    inline void setState_( int newState ) throw()
    {
        m_state = newState;
    }

   /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is use to get the Subscriber's sequence number
      */
    inline uint16_t getSequenceNumber_() const throw()
    {
        return m_seqNumber;
    }

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is use to set the Subscriber's sequence number
      */
    inline void setSequenceNumber_( uint16_t newSeqNumber ) throw()
    {
        m_seqNumber = newSeqNumber;
    }

public:
    /// Virtual destructor
    virtual ~SubscriberApi() {}
};

/////////////////////////////////////////////////////////////////////////////

/** This template class defines a type safe Subscriber

    Template Arguments:
        MP      - The concrete Model Point Type
 */
template <class MP>
class Subscriber : public SubscriberApi
{
public:
    /// Type safe change notification.  See Cpl::Rte::SubscriberApi
    virtual void modelPointChanged( MP& modelPointThatChanged ) throw() = 0;

public:
    /// Constructor
    Subscriber() {}

protected:
    /// See Cpl::Rte::SubscriberApi
    void genericModelPointChanged( ModelPoint& modelPointThatChanged ) throw() { modelPointChanged( *((MP*) &modelPointThatChanged) ); }
};



};      // end namespaces
};
#endif  // end header latch
