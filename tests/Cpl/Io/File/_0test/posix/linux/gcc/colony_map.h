#ifndef COLONY_MAP_H_
#define COLONY_MAP_H_

// Cpl::System mappings
#if defined(BUILD_VARIANT_POSIX) || defined(BUILD_VARIANT_POSIX64)
#include "Cpl/System/Posix/mappings_.h"
#endif
#ifdef BUILD_VARIANT_CPP11

#include <limits.h>
#define CPL_IO_FILE_NATIVE_DIR_SEP_MAP          '/'
#define CPL_IO_FILE_MAX_NAME_MAP                PATH_MAX
#include "Cpl/System/Cpp11/mappings_.h"

#endif

// strapi mapping
#include "Cpl/Text/_mappings/_posix/strapi.h"


#endif

