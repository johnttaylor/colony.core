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


#include "RefCounter.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Rte::Mp;

///////////////////////////////////////////////////////////////////////////////
RefCounter::RefCounter( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo )
    :Basic<uint32_t>( myModelBase, staticInfo )
{
}

RefCounter::RefCounter( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, uint32_t initialValue )
    : Basic<uint32_t>( myModelBase, staticInfo, initialValue )
{
}


///////////////////////////////////////////////////////////////////////////////
int8_t RefCounter::read( uint32_t& dstData, uint16_t* seqNumPtr ) const throw()
{
    return ModelPointCommon_::read( &dstData, sizeof( uint32_t ), seqNumPtr );
}

uint16_t RefCounter::reset( uint32_t newValue, LockRequest_T lockRequest ) throw()
{
    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Increment the counter and prevent overflow
        uint32_t previous = m_data;
        m_data            = newValue;

        // Generate change notices on transition to valid OR zero-to-not-zero OR not-zero-to-zero
        if ( !IS_VALID( m_validState ) || (previous == 0 && m_data != 0) || (m_data == 0 && previous != 0) )
        {
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t RefCounter::increment( uint32_t incrementAmount, LockRequest_T lockRequest ) throw()
{
    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Increment the counter and prevent overflow
        uint32_t previous = m_data;
        m_data           += incrementAmount;
        if ( m_data < previous )
        {
            m_data = (uint32_t) -1;
        }

        // Generate change notices on transition to valid OR zero-to-not-zero
        if ( !IS_VALID( m_validState ) || (previous == 0 && m_data != 0) )
        {
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t RefCounter::decrement( uint32_t decrementAmount, LockRequest_T lockRequest ) throw()
{
    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Decrement the counter and prevent underflow
        uint32_t previous = m_data;
        m_data           -= decrementAmount;
        if ( m_data > previous )
        {
            m_data = 0;
        }

        // Generate change notices on transition to valid OR not-zero-to-zero
        if ( !IS_VALID( m_validState ) || (m_data == 0 && previous != 0) )
        {
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t RefCounter::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) throw()
{
    m_modelDatabase.lock_();
    m_data          = 0;
    uint16_t result = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
    m_modelDatabase.unlock_();
    return result;
}

void RefCounter::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void RefCounter::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* RefCounter::getTypeAsText() const throw()
{
    return "Cpl::Rte::Mp::RefCounter";
}

bool RefCounter::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const throw()
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
        dst.formatOpt( append, "%lu", (unsigned long) value );
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    return true;
}

const char* RefCounter::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    const char*   result = 0;
    uint16_t      seqnum = SEQUENCE_NUMBER_UNKNOWN;
    const char*   endptr;
    unsigned long value;

    // Check for increment/decrement operation
    bool increment = false;
    bool decrement = false;
    if ( *srcText == '+' )
    {
        increment = true;
        srcText++;
    }
    else if ( *srcText == '-' )
    {
        decrement = true;
        srcText++;
    }

    // Convert the numeric value
    if ( Cpl::Text::a2ul( value, srcText, 10, terminationChars, &endptr ) && value <= ULONG_MAX )
    {
        result = endptr;

        if ( increment )
        {
            seqnum = RefCounter::increment( value, lockAction );
        }
        else if ( decrement )
        {
            seqnum = RefCounter::decrement( value, lockAction );
        }
        else
        {
            seqnum = RefCounter::reset( value, lockAction );
        }
    }

    // Conversion failed!
    else
    {
        if ( errorMsg )
        {
            errorMsg->format( "Conversion of %s[%s] to a uint32_t failed OR value too great.", getTypeAsText(), srcText );
        }
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

