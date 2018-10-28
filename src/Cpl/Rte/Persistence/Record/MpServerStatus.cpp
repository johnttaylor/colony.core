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


#include "MpServerStatus.h"


///
using namespace Cpl::Rte::Persistence::Record;

///////////////////////////////////////////////////////////////////////////////
MpServerStatus::MpServerStatus( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo )
    :Cpl::Rte::Mp::Enum<ServerStatus>( myModelBase, staticInfo )
{
}

MpServerStatus::MpServerStatus( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, ServerStatus initialValue )
    : Cpl::Rte::Mp::Enum<ServerStatus>( myModelBase, staticInfo, initialValue )
{
}

///////////////////////////////////////////////////////////////////////////////
const char* MpServerStatus::getTypeAsText() const throw()
{
    return "Cpl::Rte::Persistence::Record::ServerStatus";
}

void MpServerStatus::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpServerStatus::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}
