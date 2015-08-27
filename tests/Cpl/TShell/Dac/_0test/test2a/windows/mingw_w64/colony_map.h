#ifndef COLONY_MAP_H_
#define COLONY_MAP_H_

/// Null BSP
#include "Bsp/UserSpace/Api.h"

// Cpl::System mappings
#ifdef BUILD_VARIANT_CPP11
#include "Cpl/System/Cpp11/mappings_.h"

#else
#include "Cpl/System/Win32/mappings_.h"
#endif

// strapi mapping
#include "Cpl/Text/_mappings/mingw/strapi.h"


#endif
