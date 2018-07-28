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

#include "EventApi.h"
#include "Cpl/System/FatalError.h"


#define RESERVED_BIT_NUM        ((Cpl_Itc_EventFlags_T)((sizeof(Cpl_Itc_EventFlags_T)*8)-1))
#define RESERVED_BIT_MASK       ((Cpl_Itc_EventFlags_T)(1<<RESERVED_BIT_NUM))


/// 
using namespace Cpl::Itc;


////////////////////////////////////////////////////////////////////////////////
void EventApi::notifyEvents( Cpl_Itc_EventFlags_T events ) throw()
{
    if ( (events & RESERVED_BIT_MASK) == RESERVED_BIT_MASK )
    {
        Cpl::System::FatalError::logf( "Cpl::Itc::EventApi::notifyEvents( %lX ) - OUT-OF-RANGE event number", (unsigned long) events );
        return;
    }

    internalNotify_( events );
}

void EventApi::notify( uint8_t eventNumber ) throw()
{
    if ( eventNumber >= RESERVED_BIT_NUM )
    {
        Cpl::System::FatalError::logf( "Cpl::Itc::EventApi::notify( %u ) - OUT-OF-RANGE event number", eventNumber );
        return;
    }

    internalNotify_( 1 << eventNumber );
}


void EventApi::su_notifyEvents( Cpl_Itc_EventFlags_T events ) throw()
{
    if ( (events & RESERVED_BIT_MASK) == RESERVED_BIT_MASK )
    {
        Cpl::System::FatalError::log( "Cpl::Itc::EventApi::notifyEvents() - OUT-OF-RANGE event number", events );
        return;
    }

    internalNotify_( events );
}

void EventApi::su_notify( uint8_t eventNumber ) throw()
{
    if ( eventNumber >= RESERVED_BIT_NUM )
    {
        Cpl::System::FatalError::log( "Cpl::Itc::EventApi::notify() - OUT-OF-RANGE event number", eventNumber );
        return;
    }

    su_internalNotify_( 1 << eventNumber );
}

