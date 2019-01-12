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


#include "Subscriber.h"
#include "Cpl/System/FatalError.h"

using namespace Cpl::Rte;



/////////////////////////////
SubscriberBase::SubscriberBase( Cpl::Rte::MailboxServer& myMailbox )
    : m_state( 0 )
    , m_point( 0 )
    , m_mailboxHdl( myMailbox )
    , m_seqNumber( ModelPoint::SEQUENCE_NUMBER_UNKNOWN )
{
}

NotificationApi_* SubscriberBase::getNotificationApi_() const throw()
{
    return &m_mailboxHdl;
}

void SubscriberBase::setModelPoint_( ModelPoint* modelPoint ) throw()
{
    m_point = modelPoint;
}

int SubscriberBase::getState_() const throw()
{
    return m_state;
}

void SubscriberBase::setState_( int newState ) throw()
{
    m_state = newState;
}

uint16_t SubscriberBase::getSequenceNumber_() const throw()
{
    return m_seqNumber;
}

void SubscriberBase::setSequenceNumber_( uint16_t newSeqNumber ) throw()
{
    m_seqNumber = newSeqNumber;
}

ModelPoint* SubscriberBase::getModelPoint_() throw()
{
    if ( m_point == 0 )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Subscriber().  Protocol Error.  getModelPoint() was called before setModelPoint()" );
        return 0;
    }

    return m_point;
}