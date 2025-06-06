#ifndef Cpl_Text_x_mappings_mingw_strapi_h_
#define Cpl_Text_x_mappings_mingw_strapi_h_
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

	This file provides the mapping of the 'strapi' to the MinGW C/C++ compiler.

	The MinGW compiler provides all of the strapi natively.

 */




 ///////////////////////////////////////////////////////////////////////////////

#include <string.h>

#define HAVE_CPL_TEXT_STRCASECMP
#define HAVE_CPL_TEXT_STRNCASECMP
#define HAVE_CPL_TEXT_STRUPR
#define HAVE_CPL_TEXT_STRLWR
#define HAVE_CPL_TEXT_STRSET
#define HAVE_CPL_TEXT_STRNSET
#define HAVE_CPL_TEXT_STRREV
#define HAVE_CPL_TEXT_STRTOK_R
#define HAVE_CPL_TEXT_SNPRINTF
#define HAVE_CPL_TEXT_VSNPRINTF      



#endif  // end header latch
