/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Itc/_0test/WriteSync.h"
#include "Cpl/Itc/SyncReturnHandler.h"


///////////////////
WriteSync::WriteSync( Cpl::Itc::PostApi& myMbox )
    :m_mbox( myMbox )
{
}


///////////////////
void WriteSync::write( int newValue ) noexcept
{
    WritePayload                  payload( newValue );
    Cpl::Itc::SyncReturnHandler   srh;
    WriteMsg 	                  msg( *this, payload, srh );
    m_mbox.postSync( msg );
}
