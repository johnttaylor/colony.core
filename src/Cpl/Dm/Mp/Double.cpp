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


#include "Double.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Math/real.h"
#include <limits.h>

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
Double::Double( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :Basic<double>( myModelBase, staticInfo )
{
}

Double::Double( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, double initialValue )
    : Basic<double>( myModelBase, staticInfo, initialValue )
{
}

///////////////////////////////////////////////////////////////////////////////
int8_t Double::read( double& dstData, uint16_t* seqNumPtr ) const throw()
{
    return ModelPointCommon_::read( &dstData, sizeof( double ), seqNumPtr );
}

uint16_t Double::write( double newValue, LockRequest_T lockRequest ) throw()
{
    return ModelPointCommon_::write( &newValue, sizeof( double ), lockRequest );
}

uint16_t Double::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Double::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Double::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}

bool Double::isDataEqual_( const void* otherData ) const throw()
{
    double left = *((double*) otherData);
    return Cpl::Math::areDoublesEqual( m_data, left );
}

///////////////////////////////////////////////////////////////////////////////
const char* Double::getTypeAsText() const throw()
{
    return "Cpl::Dm::Mp::Double";
}

bool Double::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const throw()
{
    // Get a snapshot of the my data and state
    m_modelDatabase.lock_();
    double   value  = m_data;
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;
    m_modelDatabase.unlock_();

    // Convert data and state to a string
    if ( convertStateToText( dst, append, locked, valid ) )
    {
        dst.formatOpt( append, "%g", value );
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    return true;
}

const char* Double::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    const char*   result = 0;
    const char*   endptr;
    double        value;
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;
    m_modelDatabase.unlock_();

    if ( Cpl::Text::a2d( value, srcText, terminationChars, &endptr ) && value >= -DBL_MAX && value <= DBL_MAX )
    {
        seqnum = write( value, lockAction );
        result = endptr;
    }

    // Conversion failed!
    else
    {
        if ( errorMsg )
        {
            errorMsg->format( "Conversion of %s[%s] to a double failed OR value out-of-range.", getTypeAsText(), srcText );
        }
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

