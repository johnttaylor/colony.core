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


#include "Uint32.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Rte::Mp;

///////////////////////////////////////////////////////////////////////////////
Uint32::Uint32( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, bool decimalFormat, int8_t validState, uint32_t initialValue )
    :Basic<uint32_t>( myModelBase, staticInfo, initialValue, validState )
    , m_decimal( decimalFormat )
{
}

///////////////////////////////////////////////////////////////////////////////
uint16_t Uint32::read( uint32_t& dstData, int8_t& validState ) const throw()
{
    return ModelPointCommon_::read( &dstData, sizeof( uint32_t ), validState );
}

uint16_t Uint32::write( uint32_t newValue, LockRequest_T lockRequest ) throw()
{
    return ModelPointCommon_::write( &newValue, sizeof( uint32_t ), lockRequest );
}

uint16_t Uint32::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Uint32::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Uint32::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* Uint32::getTypeAsText() const throw()
{
    return m_decimal? "Cpl::Rte::Mp::Uint32-dec": "Cpl::Rte::Mp::Uint32-hex";
}

bool Uint32::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const throw()
{
    // Get a snapshot of the my data and state
    m_modelDatabase.lock_();
    uint32_t value  = m_data;
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;
    m_modelDatabase.unlock_();

    // Convert data and state to a string
    if ( convertStateToText( dst, append, locked, valid ) )
    {
        if ( m_decimal )
        {
            dst.formatOpt( append, "%lu", (unsigned long) value );
        }
        else
        {
            dst.formatOpt( append, "0x%lX", (unsigned long) value );
        }
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    return true;
}

const char* Uint32::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    const char*   result = 0;
    uint16_t      seqnum = SEQUENCE_NUMBER_UNKNOWN;
    const char*   endptr;
    unsigned long value;
    if ( Cpl::Text::a2ul( value, srcText, m_decimal ? 10 : 16, terminationChars, &endptr ) && value <= ULONG_MAX )
    {
        seqnum = write( value, lockAction );
        result = endptr;
    }

    // Conversion failed!
    else
    {
        if ( errorMsg )
        {
            errorMsg->format( "Conversion of %s[%s] (base=%d) to a uint32_t failed OR value too great.", getTypeAsText(), srcText, m_decimal ? 10 : 16 );
        }
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}
