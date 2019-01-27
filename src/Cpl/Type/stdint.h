#ifndef Cpl_Type_stdint_h_
#define Cpl_Type_stdint_h_
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
/** @file 

    This file contains is a 'Hack' to provide support for the stdint.h header
    file which is NOT part of the OFFICIAL C++03 standard (not official till
    C++11).  Most C++03 compilers support stdint.h, but not all or not 100%
    compliance with the C99 standard definition of stdint.h.  

    NOTE: These file/interface/symbols/etc. is the GLOBAL name space.

*/

/// Helper macro
#ifndef     __MY_CONCATenate
#define 	__MY_CONCATenate(left, right)   left ## right
#endif
 
/// Helper macro
#ifndef     __MY_CONCAT
#define 	__MY_CONCAT(left, right)   __MY_CONCATenate(left, right)
#endif


/// Limits
#ifndef     INT8_MAX
#define 	INT8_MAX   0x7f
#endif

/// Limits
#ifndef     INT8_MIN
#define 	INT8_MIN   (-INT8_MAX - 1)
#endif
 
/// Limits
#ifndef     UINT8_MAX
#define 	UINT8_MAX   (INT8_MAX * 2 + 1)
#endif
 
/// Limits
#ifndef     INT16_MAX
#define 	INT16_MAX   0x7fff
#endif
 
/// Limits
#ifndef     INT16_MIN
#define 	INT16_MIN   (-INT16_MAX - 1)
#endif
 
/// Limits
#ifndef     UINT16_MAX
#define 	UINT16_MAX   (__MY_CONCAT(INT16_MAX, U) * 2U + 1U)
#endif
 
/// Limits
#ifndef     INT32_MAX
#define 	INT32_MAX   0x7fffffffL
#endif
 
/// Limits
#ifndef     INT32_MIN
#define 	INT32_MIN   (-INT32_MAX - 1L)
#endif
 
/// Limits
#ifndef     UINT32_MAX
#define 	UINT32_MAX   (__MY_CONCAT(INT32_MAX, U) * 2UL + 1UL)
#endif
 
/// Limits
#ifndef     INT64_MAX
#define 	INT64_MAX   0x7fffffffffffffffLL
#endif
 
/// Limits
#ifndef     INT64_MIN
#define 	INT64_MIN   (-INT64_MAX - 1LL)
#endif
 
/// Limits
#ifndef     UINT64_MAX
#define 	UINT64_MAX   (__MY_CONCAT(INT64_MAX, U) * 2ULL + 1ULL)
#endif



#endif  // end header latch

