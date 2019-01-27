#ifndef Cpl_System_Cpp11_mappings_x_h_
#define Cpl_System_Cpp11_mappings_x_h_
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
/** @file

    This file provides the mappings for Platform specific types, etc. needed
    by Cpl::System

 */

#include "colony_config.h"  // Note: Yet another work around for managing OS specific items not covered by C++11, e.g. the native directory separator


#include <mutex>
#include <condition_variable>
#include <time.h>
#include <stdlib.h>
#include <thread>
#include <assert.h>

/// C++11  Mapping
#define CPL_SYSTEM_ASSERT_MAP(e)                assert(e)

/// C++11 Mapping
#define Cpl_System_Thread_NativeHdl_T_MAP       std::thread::native_handle_type

/// C++11 Mapping
#define Cpl_System_Mutex_T_MAP                  std::recursive_mutex

/// C++11 Mapping
#define Cpl_System_FastLock_T_MAP               std::mutex // Not sure if this is faster than a recursive_mutex -->but its worth a try

/// No native Semaphore in C++11 -->so build one out of a condition variable
typedef struct Cpl_System_Cpp11_Sema_Tag
{
    ///
    std::mutex              m_mutex;
    ///
    std::condition_variable m_cond;
    ///
    unsigned                m_count;
} Cpl_System_Cpp11_Sema_T;


/// C++11 Mapping
#define Cpl_System_Sema_T_MAP                   Cpl_System_Cpp11_Sema_T


/// C++11 Mapping
#define Cpl_System_TlsKey_T_MAP                 int


/// C++11 Mapping
#ifndef CPL_SYSTEM_SHELL_SUPPORTED_x_MAP
#define CPL_SYSTEM_SHELL_SUPPORTED_x_MAP        0   // Default to NOT supported (since this requires knowledge of the native OS)
#endif 

/// Default the native new line to the standard newline
#ifndef CPL_IO_NEW_LINE_NATIVE_MAP
#define CPL_IO_NEW_LINE_NATIVE_MAP              "\n"
#endif


/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP      0

/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       0

/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_LOWEST_MAP       0

/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_RAISE_MAP        0

/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_LOWER_MAP        0


/// Hack because Microsoft's VC12 only HAS PARTIAL support for thread_local
#if _MSC_VER >= 1800
#define  thread_local __declspec(thread)
#endif


#endif  // end header latch
