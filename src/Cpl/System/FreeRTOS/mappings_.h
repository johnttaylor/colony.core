#ifndef Cpl_System_FreeRtos_mappings_x_h_
#define Cpl_System_FreeRtos_mappings_x_h_
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
/** @file

    This file provides the mappings for Platform specific types, etc. needed
    by Cpl::System

 */


#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "Cpl/System/_assert/c_assert.h"

// PRETTY_FUNCTION macro is non-standard
#if defined(__GNUC__)
/// Take advantage of GCC's pretty function symbol
#define CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME    __PRETTY_FUNCTION__

#elif defined(_MSC_VER)
/// Take advantage of Microsoft's pretty function symbol
#define CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME    __FUNCSIG__

#else
/// Use C++11 function name
#define CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME    __func__
#endif  // end __PRETTY_FUNCTION__


/// Mapping
#define CPL_SYSTEM_ASSERT_MAP(e)                cpl_system_assert_c_wrapper( e, __FILE__, __LINE__, CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME )

/// Mapping
#define Cpl_System_Thread_NativeHdl_T_MAP       TaskHandle_t  

/// Mapping
#define Cpl_System_Mutex_T_MAP                  SemaphoreHandle_t

/// Mapping
#define Cpl_System_Sema_T_MAP                   SemaphoreHandle_t

/// Mapping
#define Cpl_System_TlsKey_T_MAP                 uint8_t

/// Mapping 
#define CPL_SYSTEM_SHELL_NULL_DEVICE_x_MAP      "" // Since the 'shell' is not support -->the value has no meaning

/// Mapping 
#define CPL_SYSTEM_SHELL_SUPPORTED_x_MAP        0

/// Mapping
#define CPL_IO_NEW_LINE_NATIVE_MAP              "\n"
//
/////
//#define CPL_IO_FILE_NATIVE_DIR_SEP_MAP          '/'
//
/////
//#define CPL_IO_FILE_MAX_NAME_MAP                PATH_MAX


/// Mapping
inline unsigned long CplSystemElapsedTime_getTimeInMilliseconds_MAP()
{
    return xTaskGetTickCount() / portTICK_PERIOD_MS;
}


//
// Thread Priorities
//    The maximum number of priority levels is defined FreeRTOSConfig.h
//    The highest priority is 'configMAX_PRIORITIES - 1', the lowest
//    priority is 1.
//
/// Mapping
#define CPL_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP      (configMAX_PRIORITIES - 1)
/// Mapping
#define CPL_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       (configMAX_PRIORITIES/2)
/// Mapping
#define CPL_SYSTEM_THREAD_PRIORITY_LOWEST_MAP       1
/// Mapping
#define CPL_SYSTEM_THREAD_PRIORITY_RAISE_MAP        (1)
/// Mapping
#define CPL_SYSTEM_THREAD_PRIORITY_LOWER_MAP        (-1)




#endif  // end header latch

