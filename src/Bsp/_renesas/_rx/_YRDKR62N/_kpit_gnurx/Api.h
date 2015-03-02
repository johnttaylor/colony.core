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

*----------------------------------------------------------------------------*/ 


#include "colony_config.h"
#include "colony_map.h"
#include "Bsp/_renesas/_rx/_YRDKR62N/iodefine.h"
#include <stdint.h>



//////////////////////////////////////////////////////////
/*
** BOARD Specific APIs
*/

//////////////////////////
/// System Clocks.
///
/// NOTE: The clock settings are adjustable via the 'colony.config.h' header file
//////////////////////////


// CPU's main crystal frequency
#ifndef     BSP_SYSCLK_X1_XTAL_FREQUENCY
#define     BSP_SYSCLK_X1_XTAL_FREQUENCY        (12000000L)
#endif

/// Realtime clock crystal
#ifndef     BSP_SYSCLK_X2_XTAL_FREQUENCY
#define     BSP_SYSCLK_X2_XTAL_FREQUENCY        (32768)
#endif

/// Multipler for the internal clock
#ifndef     BSP_SYSCLK_ICLK_MUL
#define     BSP_SYSCLK_ICLK_MUL                 (8)              
#endif

/// Multipler for the peripheral clock
#ifndef     BSP_SYSCLK_PCLK_MUL
#define     BSP_SYSCLK_PCLK_MUL                 (4)
#endif

/// Multipler for the external bus clock
#ifndef     BSP_SYSCLK_BCLK_MUL
#define     BSP_SYSCLK_BCLK_MUL                 (4)
#endif

/// System clock frequency
#define     BSP_SYSCLK_ICLK_FREQUENCY           (BSP_SYSCLK_X1_XTAL_FREQUENCY * BSP_SYSCLK_ICLK_MUL)

/// Peripheral clock frequency
#define     BSP_SYSCLK_PCLK_FREQUENCY           (BSP_SYSCLK_X1_XTAL_FREQUENCY * BSP_SYSCLK_PCLK_MUL)

/// External bus clock frequency
#define     BSP_SYSCLK_BCLK_FREQUENCY           (BSP_SYSCLK_X1_XTAL_FREQUENCY * BSP_SYSCLK_BCLK_MUL)




//////////////////////////
/// LEDs
//////////////////////////

/** Turns on or off one of the 'circle' LEDs.  A index of 0 will turn on/off all
    of the 'circle' LEDs.  Index 1 maps to physical LED4, Index 12 maps to 
    physical LED15.
 */
void Bsp_setCircleLED( uint8_t ledIndex, bool setToOn );


/** Toggles one of the 'circle' LEDs.  A index of 0 will toggle all
    of the 'circle' LEDs.  Index 1 maps to physical LED4, Index 12 maps 
    to physical LED15.
 */
void Bsp_toggleCircleLED( uint8_t ledIndex );


//////////////////////////
/// Push Button Switchs
//////////////////////////

/** This method returns true if the specificed push button is
    'pressed'. Index of 1 maps to SW1, ... 3 maps to SW3.  An index
    of out range will always return false.
 */
bool Bsp_isButtonPressed( uint8_t buttonIndex );



//////////////////////////////////////////////////////////
/*
** Generic APIs
*/

/// Generic API
#define Bsp_Api_disableIrqs_MAP

/// Generic API
#define Bsp_Api_enableIrqs_MAP

/// Generic API
#define Bsp_Api_pushAndDisableIrqs_MAP

/// Generic API
#define Bsp_Api_popIrqs_MAP




#endif  // end header latch
