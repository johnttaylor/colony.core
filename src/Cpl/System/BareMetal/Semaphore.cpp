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

#include "Cpl/System/Semaphore.h"
#include "Cpl/System/GlobalLock.h"
#include "Cpl/System/ElapsedTime.h"

///
using namespace Cpl::System;



//////////////////////////////////////////////////
Semaphore::Semaphore( unsigned initialCount )
{
    m_sema = initialCount;
}

Semaphore::~Semaphore()
{
    // Nothing needed
}

int Semaphore::signal( void ) throw()
{
    GlobalLock::begin();
    su_signal();
    GlobalLock::end();
    return 0;
}

int Semaphore::su_signal( void ) throw()
{
    // Increment the semaphore (but don't let it roll over)
    m_sema++;
    if ( m_sema == 0 )
    {
        m_sema--;
    }
    return 0;
}

bool Semaphore::tryWait( void ) throw()
{
    bool signaled = false;
    GlobalLock::begin();
    if ( m_sema > 0 )
    {
        m_sema--;
        signaled = true;
    }
    GlobalLock::end();

    return signaled;
}

void Semaphore::waitInRealTime( void ) throw()
{
    for ( ;;)
    {
        GlobalLock::begin();
        if ( m_sema > 0 )
        {
            m_sema--;
            GlobalLock::end();
            return;
        }
        GlobalLock::end();
    }
}

bool Semaphore::timedWaitInRealTime( unsigned long timeout ) throw()
{
    unsigned long start = ElapsedTime::milliseconds();

    for ( ;;)
    {
        GlobalLock::begin();
        if ( m_sema > 0 )
        {
            m_sema--;
            GlobalLock::end();
            return true;
        }
        GlobalLock::end();

        if ( ElapsedTime::expiredMilliseconds( start, timeout ) )
        {
            return false;
        }
    }
}