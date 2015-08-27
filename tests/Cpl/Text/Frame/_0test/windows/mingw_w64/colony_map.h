#ifndef COLONY_MAP_H_
#define COLONY_MAP_H_

// Cpl::System mappings


#ifdef BUILD_VARIANT_CPP11

#include <stdlib.h>
#define CPL_IO_FILE_NATIVE_DIR_SEP_MAP          '\\'
#define CPL_IO_FILE_MAX_NAME_MAP                _MAX_PATH

#include "Cpl/System/Cpp11/mappings_.h"


#else
#include "Cpl/System/Win32/mappings_.h"
#endif

// strapi mapping
#include "Cpl/Text/_mappings/mingw/strapi.h"


#endif
