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


#include "Uint64.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Rte::Mp;

///////////////////////////////////////////////////////////////////////////////
Uint64::Uint64( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, bool decimalFormat )
    :Basic<uint64_t>( myModelBase, staticInfo )
    , m_decimal( decimalFormat )
{
}

Uint64::Uint64( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, uint64_t initialValue, bool decimalFormat )
    : Basic<uint64_t>( myModelBase, staticInfo, initialValue )
    , m_decimal( decimalFormat )
{
}

///////////////////////////////////////////////////////////////////////////////
int8_t Uint64::read( uint64_t& dstData, uint16_t* seqNumPtr ) const throw()
{
    return ModelPointCommon_::read( &dstData, sizeof( uint64_t ), seqNumPtr );
}

uint16_t Uint64::write( uint64_t newValue, LockRequest_T lockRequest ) throw()
{
    return ModelPointCommon_::write( &newValue, sizeof( uint64_t ), lockRequest );
}

uint16_t Uint64::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Uint64::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Uint64::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* Uint64::getTypeAsText() const throw()
{
    return m_decimal ? "Cpl::Rte::Mp::Uint64-dec" : "Cpl::Rte::Mp::Uint64-hex";
}

bool Uint64::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const throw()
{
    // Get a snapshot of the my data and state
    m_modelDatabase.lock_();
    uint64_t value  = m_data;
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;
    m_modelDatabase.unlock_();

    // Convert data and state to a string
    if ( convertStateToText( dst, append, locked, valid ) )
    {
        if ( m_decimal )
        {
            dst.formatOpt( append, "%llu", (unsigned long long) value );
        }
        else
        {
            dst.formatOpt( append, "0x%llX", (unsigned long long) value );
        }
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    return true;
}

const char* Uint64::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    const char*        result = 0;
    uint16_t           seqnum = SEQUENCE_NUMBER_UNKNOWN;
    const char*        endptr;
    unsigned long long value;
    if ( Cpl::Text::a2ull( value, srcText, m_decimal ? 10 : 16, terminationChars, &endptr ) )
    {
        seqnum = write( value, lockAction );
        result = endptr;
    }

    // Conversion failed!
    else
    {
        if ( errorMsg )
        {
            errorMsg->format( "Conversion of %s[%s] (base=%d) to a uint64_t failed.", getTypeAsText(), srcText, m_decimal ? 10 : 16 );
        }
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

