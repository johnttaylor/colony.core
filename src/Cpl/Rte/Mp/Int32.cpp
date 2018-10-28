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


#include "Int32.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Rte::Mp;

///////////////////////////////////////////////////////////////////////////////
Int32::Int32( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, bool decimalFormat )
    :Basic<int32_t>( myModelBase, staticInfo )
    , m_decimal( decimalFormat )
{
}


Int32::Int32( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, int32_t initialValue, bool decimalFormat )
    :Basic<int32_t>( myModelBase, staticInfo, initialValue )
    , m_decimal( decimalFormat )
{
}

///////////////////////////////////////////////////////////////////////////////
uint16_t Int32::read( int32_t& dstData, int8_t& validState ) const throw()
{
    return ModelPointCommon_::read( &dstData, sizeof( int32_t ), validState );
}

uint16_t Int32::write( int32_t newValue, LockRequest_T lockRequest ) throw()
{
    return ModelPointCommon_::write( &newValue, sizeof( int32_t ), lockRequest );
}

uint16_t Int32::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Int32::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Int32::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* Int32::getTypeAsText() const throw()
{
    return m_decimal ? "Cpl::Rte::Mp::Int32-dec" : "Cpl::Rte::Mp::Int32-hex";
}

bool Int32::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const throw()
{
    // Get a snapshot of the my data and state
    m_modelDatabase.lock_();
    int32_t  value  = m_data;
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;
    m_modelDatabase.unlock_();

    // Convert data and state to a string
    if ( convertStateToText( dst, append, locked, valid ) )
    {
        if ( m_decimal )
        {
            dst.formatOpt( append, "%ld", (long) value );
        }
        else
        {
            dst.formatOpt( append, "0x%lX", (long) value );
        }
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    return true;
}

const char* Int32::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    const char*   result = 0;
    uint16_t      seqnum = SEQUENCE_NUMBER_UNKNOWN;
    const char*   endptr;
    long         value;
    if ( Cpl::Text::a2l( value, srcText, m_decimal ? 10 : 16, terminationChars, &endptr ) && value <= LONG_MAX && value >= LONG_MIN )
    {
        seqnum = write( value, lockAction );
        result = endptr;
    }

    // Conversion failed!
    else
    {
        if ( errorMsg )
        {
            errorMsg->format( "Conversion of %s[%s] (base=%d) to a int32_t failed OR value too great.", getTypeAsText(), srcText, m_decimal ? 10 : 16 );
        }
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

