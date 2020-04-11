/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "RecordServer.h"
#include "Cpl/System/Assert.h"

#define SECT_ "Cpl::Persistence"


///
using namespace Cpl::Persistence;

/////////////////////
RecordServer::RecordServer( Record** recordList, unsigned long timingTickInMsec ) noexcept
    : Cpl::Dm::MailboxServer( timingTickInMsec )
    , Cpl::Itc::CloseSync( *this )
    , m_records( recordList )
    , m_opened( false )
{
    CPL_SYSTEM_ASSERT( recordList );
}

void RecordServer::request( OpenMsg& msg )
{
    if ( !m_opened )
    {
        m_opened = true;

        // Start each record
        Record* item = m_records[0];
        while ( item )
        {
            item->start( *this );
        }
    }
    msg.returnToSender();
}

void RecordServer::request( CloseMsg& msg )
{
    if ( m_opened )
    {
        m_opened = false;

        // Stop each record
        Record* item = m_records[0];
        while ( item )
        {
            item->stop();
        }
    }
    msg.returnToSender();
}

