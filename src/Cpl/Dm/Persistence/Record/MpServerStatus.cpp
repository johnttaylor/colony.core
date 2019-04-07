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


#include "MpServerStatus.h"


///
using namespace Cpl::Dm::Persistence::Record;

///////////////////////////////////////////////////////////////////////////////
MpServerStatus::MpServerStatus( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :Cpl::Dm::Mp::Enum<ServerStatus>( myModelBase, staticInfo )
{
}

MpServerStatus::MpServerStatus( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, ServerStatus initialValue )
    : Cpl::Dm::Mp::Enum<ServerStatus>( myModelBase, staticInfo, initialValue )
{
}

///////////////////////////////////////////////////////////////////////////////
const char* MpServerStatus::getTypeAsText() const noexcept
{
    return "Cpl::Dm::Persistence::Record::ServerStatus";
}

void MpServerStatus::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpServerStatus::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}
