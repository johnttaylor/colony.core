#ifndef Bsp_YRDKR62N_Api_h_
#define Bsp_YRDKR62N_Api_h_
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
/** @file 


    This file defines the Board Support Packge interfaces for the Renesas
    Demostration Kit - YRKDR62N - evaulation board.

    DO NOT include this file directly! Instead include the generic BSP
    interface - src/Bsp/Api.h - and then configure your project's 
    'colony_map.h' to include THIS file.

*/


#include "Bsp/_renesas/_rx/_YRDKR62N/iodefine.h"

//////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////

/// Generic API
#define Bsp_Api_disableIrqs_MAP

/// Generic API
#define Bsp_Api_enableIrqs_MAP

/// Generic API
#define Bsp_Api_pushAndDisableIrqs_MAP

/// Generic API
#define Bsp_Api_popIrqs_MAP




#endif  // end header latch
