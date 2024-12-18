#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Multiple Volumes
#define OPTION_CPL_IO_FILE_LITTLEFS_MAX_VOLUMES 2

// Enable Trace (but NOT on code coverage builds)
#ifndef BUILD_VARIANT_WIN32
#define USE_CPL_SYSTEM_TRACE
#endif

// Enble ASSERT macros
#define USE_CPL_SYSTEM_ASSERT_MACROS

#endif
