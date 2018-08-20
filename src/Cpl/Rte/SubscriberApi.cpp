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


#include "SubscriberApi.h"
#include "Cpl/System/FatalError.h"

using namespace Cpl::Rte;



/////////////////////////////
SubscriberApi::SubscriberApi( Cpl::Rte::MailboxServer& myMailbox )
    : m_state( 0 )
    , m_point( 0 )
    , m_mailboxHdl( myMailbox )
    , m_seqNumber( ModelPoint::SEQUENCE_NUMBER_UNKNOWN )
{
}

ModelPoint* SubscriberApi::getModelPoint_() throw()
{
    if ( m_point == 0 )
    {
        Cpl::System::FatalError::logf( "Cpl::Rte::Subscriber().  Protocol Error.  getModelPoint() was called before setModelPoint()" );
        return;
    }

    return m_point;
}
