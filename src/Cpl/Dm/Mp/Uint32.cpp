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


#include "Uint32.h"
#include <limits.h>

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
Uint32::Uint32( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, bool decimalFormat )
    :BasicNumeric<uint32_t>( myModelBase, staticInfo, decimalFormat )
{
}

Uint32::Uint32( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, uint32_t initialValue, bool decimalFormat )
    : BasicNumeric<uint32_t>( myModelBase, staticInfo, initialValue, decimalFormat )
{
}

///////////////////////////////////////////////////////////////////////////////
int8_t Uint32::read( uint32_t& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( uint32_t ), seqNumPtr );
}

uint16_t Uint32::write( uint32_t newValue, LockRequest_T lockRequest ) noexcept
{
    return ModelPointCommon_::write( &newValue, sizeof( uint32_t ), lockRequest );
}

uint16_t Uint32::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Uint32::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Uint32::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* Uint32::getTypeAsText() const noexcept
{
    return m_decimal ? "Cpl::Dm::Mp::Uint32-dec" : "Cpl::Dm::Mp::Uint32-hex";
}

