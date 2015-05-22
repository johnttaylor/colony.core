/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Threads.h"
#include <windows.h>


///
using namespace Cpl::TShell::Dac::Cmd::Win32;



///////////////////////////
Threads::Threads( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Cpl::TShell::Dac::Cmd::Threads(commandList)
    {
    }


/////////////////////////////////////////////////////////
void Threads::hookHeader1( Cpl::Text::String& text )    
    {
    text.formatAppend( "  %-3s  %20s", "Pri", "CPU Cycles" );
    }

void Threads::hookHeader2( Cpl::Text::String& text )
    {
    text.formatAppend( "  %-3s  %20s", "---", "----------" );
    }

void Threads::hookThreadEntry( Cpl::Text::String& text, Cpl::System::Thread& currentThread )
    {
    HANDLE hdl = currentThread.getNativeHandle();

    // Get Priority (and skip Win32 specific columns if an error (this will happen with C++11 threads on the 'main' thread)
    int priority = GetThreadPriority(hdl);
    if ( priority == THREAD_PRIORITY_ERROR_RETURN )
        {
        return;
        }

    // Get CPU Cycles (and skip Win32 specific.....)
    ULONG64 cycles;
    if ( !QueryThreadCycleTime( hdl, &cycles ) )
        {
        return;
        }

    text.formatAppend( "  %3d  %20llu", GetThreadPriority(hdl), cycles );
    }

