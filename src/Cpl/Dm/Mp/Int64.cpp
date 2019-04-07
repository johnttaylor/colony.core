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


#include "Int64.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
Int64::Int64( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, bool decimalFormat )
    :Basic<int64_t>( myModelBase, staticInfo )
    , m_decimal( decimalFormat )
{
}

Int64::Int64( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, int64_t initialValue, bool decimalFormat )
    : Basic<int64_t>( myModelBase, staticInfo, initialValue )
    , m_decimal( decimalFormat )
{
}

///////////////////////////////////////////////////////////////////////////////
int8_t Int64::read( int64_t& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( int64_t ), seqNumPtr );
}

uint16_t Int64::write( int64_t newValue, LockRequest_T lockRequest ) noexcept
{
    return ModelPointCommon_::write( &newValue, sizeof( int64_t ), lockRequest );
}

uint16_t Int64::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Int64::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Int64::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* Int64::getTypeAsText() const noexcept
{
    return m_decimal ? "Cpl::Dm::Mp::Int64-dec" : "Cpl::Dm::Mp::Int64-hex";
}

bool Int64::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const noexcept
{
    // Get a snapshot of the my data and state
    m_modelDatabase.lock_();
    int64_t  value  = m_data;
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;
    m_modelDatabase.unlock_();

    // Convert data and state to a string
    if ( convertStateToText( dst, append, locked, valid ) )
    {
        if ( m_decimal )
        {
            dst.formatOpt( append, "%lld", (long long) value );
        }
        else
        {
            dst.formatOpt( append, "0x%llX", (long long) value );
        }
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    return true;
}

const char* Int64::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) noexcept
{
    const char*   result = 0;
    const char*   endptr;
    long long     value;
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;
    m_modelDatabase.unlock_();

    if ( Cpl::Text::a2ll( value, srcText, m_decimal ? 10 : 16, terminationChars, &endptr ) )
    {
        seqnum = write( value, lockAction );
        result = endptr;
    }

    // Conversion failed!
    else
    {
        if ( errorMsg )
        {
            errorMsg->format( "Conversion of %s[%s] (base=%d) to a int64_t failed.", getTypeAsText(), srcText, m_decimal ? 10 : 16 );
        }
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

