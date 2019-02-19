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

#include "Cpl/System/GlobalLock.h"
#include "Cpl/System/BareMetal/Hal_.h"
#include <time.h>
#include <windows.h>

/// 
using namespace Cpl::System::BareMetal;


///////////////////////////////////////////////////////////////
void initialize( void ) throw()
{
    // Nothing needed
}

void busyWait( unsigned long waitTimeMs ) throw()
{
    Sleep( (DWORD) milliseconds );
}

unsigned long getElapsedTime( void ) throw()
{
    return clock();
}


///////////////////////////////////////////////////////////////
void Cpl::System::GlobalLock::begin( void )
{
    // Do nothing
}

void Cpl::System::GlobalLock::end( void )
{
    // Do nothing
}