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


#include "Bool.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Rte::Mp;

///////////////////////////////////////////////////////////////////////////////
Bool::Bool( Cpl::Rte::ModelDatabase& myModelBase, Cpl::Rte::StaticInfo& staticInfo, int8_t validState, bool initialValue )
    :Basic<bool>( myModelBase, staticInfo, initialValue, validState )
{
}

///////////////////////////////////////////////////////////////////////////////
uint16_t Bool::read( bool& dstData, int8_t& validState ) const throw()
{
    return ModelPointCommon_::read( &dstData, sizeof( bool ), validState );
}

uint16_t Bool::write( bool newValue, LockRequest_T lockRequest ) throw()
{
    return ModelPointCommon_::write( &newValue, sizeof( bool ), lockRequest );
}

uint16_t Bool::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Bool::attach( Observer& observer, uint16_t initialSeqNumber ) throw()
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Bool::detach( Observer& observer ) throw()
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* Bool::getTypeAsText() const throw()
{
    return "Cpl::Rte::Mp::Bool";
}

bool Bool::toString( Cpl::Text::String& dst, bool append, uint16_t* retSequenceNumber ) const throw()
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

const char* Bool::setFromText( const char* srcText, LockRequest_T lockAction, const char* not_usedterminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    const char*   result = 0;
    uint16_t      seqnum = SEQUENCE_NUMBER_UNKNOWN;
    const char*   endptr;
    bool          value;
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

