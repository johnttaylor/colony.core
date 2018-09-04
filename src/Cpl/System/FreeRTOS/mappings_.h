#ifndef Cpl_System_FreeRtos_mappings_x_h_
#define Cpl_System_FreeRtos_mappings_x_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014 John T. Taylor                                        
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
#include "Cpl/System/FatalError.h"


/// Mapping
#define CPL_SYSTEM_ASSERT_MAP(e)                if (!(e)) { Cpl::System::FatalError::logf( "ASSERT Failed at: file=%s, line=%d, func=%s\n", __FILE__, __LINE__, __FUNC__ );}

/// Mapping
#define Cpl_System_Thread_NativeHdl_T_MAP       TaskHandle_t  

/// Mapping
#define Cpl_System_Mutex_T_MAP                  SemaphoreHandle_t

/// Mapping
#define Cpl_System_FastLock_T_MAP               uint8_t // Note: Not really used, but need something or the code won't compile
                                                
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

