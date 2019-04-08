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


#include "ArrayUint8.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/atob.h"
#include <stdarg.h>


///
using namespace Cpl::Dm::Mp;


///////////////////////////////////////////////////////////////////////////////
ArrayUint8::ArrayUint8( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, size_t numElements, bool decimalFormat )
    :Array<uint8_t>( myModelBase, staticInfo, numElements, decimalFormat )
{
}

ArrayUint8::ArrayUint8( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, size_t numElements, const uint8_t* srcData, bool decimalFormat )
    :Array<uint8_t>( myModelBase, staticInfo, numElements, srcData, decimalFormat )
{
}


///////////////////////////////////////////////////////////////////////////////
int8_t ArrayUint8::read( uint8_t* dstData, size_t dstNumElements, size_t srcIndex, uint16_t* seqNumPtr ) const noexcept
{
    InternalData dst = { dstData, dstNumElements, srcIndex };
    return ModelPointCommon_::read( &dst, sizeof( dst ), seqNumPtr );
}

uint16_t ArrayUint8::write( uint8_t* srcData, size_t srcNumElements, LockRequest_T lockRequest, size_t dstIndex ) noexcept
{
    InternalData src = { srcData, srcNumElements, dstIndex };
    return ModelPointCommon_::write( &src, sizeof( src ), lockRequest );
}

uint16_t ArrayUint8::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}


///////////////////////////////////////////////////////////////////////////////
void ArrayUint8::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void ArrayUint8::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* ArrayUint8::getTypeAsText() const noexcept
{
    return m_decimal? "Cpl::Dm::Mp::ArrayUint8-dec" :"Cpl::Dm::Mp::ArrayUint8-hex";
}



