/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Cpl/System/Semaphore.h"
#include <semaphore.h>

///
using namespace Cpl::System;



//////////////////////////////////////////////////
void Semaphore::wait( void ) noexcept
{
    sem_wait( &m_sema );
}


bool Semaphore::timedWait( unsigned long timeout ) noexcept
{
    return timedWaitInRealTime( timeout );
}

