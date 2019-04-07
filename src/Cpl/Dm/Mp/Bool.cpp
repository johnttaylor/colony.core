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


#include "Bool.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
Bool::Bool( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :Basic<bool>( myModelBase, staticInfo )
{
}


Bool::Bool( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, bool initialValue )
    : Basic<bool>( myModelBase, staticInfo, initialValue )
{
}


///////////////////////////////////////////////////////////////////////////////
int8_t Bool::read( bool& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( bool ), seqNumPtr );
}

uint16_t Bool::write( bool newValue, LockRequest_T lockRequest ) noexcept
{
    return ModelPointCommon_::write( &newValue, sizeof( bool ), lockRequest );
}

uint16_t Bool::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Bool::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Bool::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* Bool::getTypeAsText() const noexcept
{
    return "Cpl::Dm::Mp::Bool";
}

bool Bool::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const noexcept
{
    // Get a snapshot of the my data and state
    m_modelDatabase.lock_();
    bool     value  = m_data;
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;
    m_modelDatabase.unlock_();

    // Convert data and state to a string
    if ( convertStateToText( dst, append, locked, valid ) )
    {
        dst.formatOpt( append, "%s", value ? "true" : "false" );
    }

    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    return true;
}

const char* Bool::setFromText( const char* srcText, LockRequest_T lockAction, const char* not_usedterminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) noexcept
{
    const char*   result = 0;
    const char*   endptr;
    bool          value;
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;
    m_modelDatabase.unlock_();

    if ( Cpl::Text::a2b( value, srcText, "true", "false", &endptr ) )
    {
        seqnum = write( value, lockAction );
        result = endptr;
    }

    // Conversion failed!
    else
    {
        if ( errorMsg )
        {
            errorMsg->format( "Conversion of %s[%s] to a bool failed.", getTypeAsText(), srcText );
        }
    }

    // Housekeeping
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }
    return result;
}

