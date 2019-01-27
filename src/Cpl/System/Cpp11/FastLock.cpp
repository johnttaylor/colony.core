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

#include "Cpl/System/FastLock.h"


//////////////////////////////////////////////////////////////////////////////
Cpl::System::FastLock::FastLock( void )
{
}


Cpl::System::FastLock::~FastLock( void )
{
}

//////////////////////////////////////////////////////////////////////////////
void Cpl::System::FastLock::lock( void )
{
    m_flock.lock();
}


void Cpl::System::FastLock::unlock( void )
{
    m_flock.unlock();
}




