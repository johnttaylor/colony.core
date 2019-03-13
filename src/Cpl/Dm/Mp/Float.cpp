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


#include "Float.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Math/real.h"
#include <limits.h>

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
Float::Float( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :Basic<float>( myModelBase, staticInfo )
{
}

Float::Float( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, float initialValue )
    : Basic<float>( myModelBase, staticInfo, initialValue )
{
}

///////////////////////////////////////////////////////////////////////////////
int8_t Float::read( float& dstData, uint16_t* seqNumPtr ) const throw()
{
    return ModelPointCommon_::read( &dstData, sizeof( float ), seqNumPtr );
}

uint16_t Float::write( float newValue, LockRequest_T lockRequest ) throw()
{
    return ModelPointCommon_::write( &newValue, sizeof( float ), lockRequest );
}

uint16_t Float::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Float::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Float::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}

bool Float::isDataEqual_( const void* otherData ) const throw()
{
    float left = *((float*) otherData);
    return Cpl::Math::areFloatsEqual( m_data, left );
}

///////////////////////////////////////////////////////////////////////////////
const char* Float::getTypeAsText() const throw()
{
    return "Cpl::Dm::Mp::Float";
}

bool Float::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const throw()
{
    // Get a snapshot of the my data and state
    m_modelDatabase.lock_();
    float    value  = m_data;
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;
    m_modelDatabase.unlock_();

    // Convert data and state to a string
    if ( convertStateToText( dst, append, locked, valid ) )
    {
        dst.formatOpt( append, "%g", (double) value );
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    return true;
}

const char* Float::setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    const char*   result = 0;
    const char*   endptr;
    double        value;
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;
    m_modelDatabase.unlock_();

    if ( Cpl::Text::a2d( value, srcText, terminationChars, &endptr ) && value >= -FLT_MAX && value <= FLT_MAX )
    {
        seqnum = write( (float) value, lockAction );
        result = endptr;
    }

    // Conversion failed!
    else
    {
        if ( errorMsg )
        {
            errorMsg->format( "Conversion of %s[%s] to a float failed OR value out-of-range.", getTypeAsText(), srcText );
        }
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

