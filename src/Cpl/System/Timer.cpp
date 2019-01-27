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

#include "Timer.h"


///
using namespace Cpl::System;

/////////////////////////////
Timer::Timer( TimerManager& timingSource )
    :m_timingSource( timingSource )
    , m_count( 0 )
{
}


/////////////////////////////
void Timer::start( unsigned long timerDurationInMilliseconds ) throw()
{

    m_timingSource.detach( *this );
    m_count = m_timingSource.msecToCounts( timerDurationInMilliseconds );
    m_timingSource.attach( *this );
}

void Timer::stop() throw()
{
    m_timingSource.detach( *this );
}


void Timer::decrement( unsigned long milliseconds ) throw()
{
    m_count -= milliseconds;
}
void Timer::increment( unsigned long milliseconds ) throw()
{
    m_count += milliseconds;
}

unsigned long Timer::count() const throw()
{
    return m_count;
}
