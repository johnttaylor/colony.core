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
Uint32::Uint32( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo* staticInfo, bool decimalFormat, uint32_t initialValue, int8_t validState )
    :Basic<uint32_t>( myModelBase, staticInfo, initialValue, validState )
    , m_decimal( decimalFormat )
{
}


///////////////////////////////////////////////////////////////////////////////
const char* Uint32::getTypeAsText() const throw()
{
    return "UINT32";
}

bool Uint32::toString( Cpl::Text::String& dst, bool append=false, uint16_t* retSequenceNumber ) const throw()
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
            dst.formatOpt( append, "%lX", (unsigned long) value );
        }
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = m_seqNum;
    }

    return true;
}

const char* Uint32::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    const char*   result = 0;
    uint16_t      seqnum = SEQUENCE_NUMBER_UNKNOWN;
    const char*   endptr;
    unsigned long value;
    if ( Cpl::Text::a2ul( value, srcText, m_decimal? 10: 16, terminationChars, &endptr ) && value <= ULONG_MAX )
    {
        uint32_t seqnum = write( value, lockAction );
        result          = endptr;
    }
    
    // Conversion failed!
    else
    {
        if ( errorMsg )
        {
            *errorMsg = "Conversion to a uint32_t failed OR value too great.";
        }
    }
    
    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

