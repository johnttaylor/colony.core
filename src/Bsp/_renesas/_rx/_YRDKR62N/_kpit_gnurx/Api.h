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
/// TIMERs
//////////////////////////

/** This funciton initializes a 16bit timer channel for a Compare-Match 
    interrupt.  In addition to calling this function, the application is 
    responsible for populating the corresponding IRQ/Vector for the CMT
    interrupt. The CMT vectors are stored in the Relocatable vector table.

    Interupt/Vector mappings:
    ------------------------
    CMT0  --> CMTU0_CMI0, Irq#27 (0x70h)
    CMT1  --> CMTU0_CMI1, Irq#28 (0x74h)
    CMT2  --> CMTU1_CMI2, Irq#29 (0x78h)
    CMT3  --> CMTU1_CMI3, Irq#30 (0x7Ch)

    
    This function can/should ONLY be called when the desired timer channel is
    in the 'stopped' state.  All timer channels after reset are considered to
    be in the 'stopped' state.


    @param channelNum       CMT channel number. 0=CMT0, 1=CMT1, 2=CMT2, 3=CMT3
    @param irqRateInHZ      Desired frequency of the CMT interrupt.  
    @param pclkDivider      PCLK divider/prescaler. 0= PCLK/8, 1=PCLK/32, 2=PCLK/128, 3=PCLK/512
    @param irqPriority      Interrupt Priority. 1=lowest, ... 15=highest

    @returns true if successful; else false is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks validates the
             range of arguments (except for irqRateInHZ which is not checked).
             There is NO checking if the actual PCLK / divider / irqRate is
             a 'valid' or accurate (i.e. a integer value).
     
    Example:
        // Sets CMT0 to generate an IRQ (CMI0) at every millisecond (1KHz) with 
        // an interrupt priority of 14.  A PCLK divider of 8 is used.
        Bsp_initCompareMatchTimer( 0, 1000, 0, 14 );  
 */  
bool Bsp_initCompareMatchTimer( uint8_t channelNum, uint32_t irqRateInHZ, uint8_t pclkDivider, uint8_t irqPriority );


/** This method starts the specified Compare Match Timer channel. 

    @param channelNum       CMT channel number. 0=CMT0, 1=CMT1, 2=CMT2, 3=CMT3

    @returns true if successful; else false is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks validates the
             range of arguments 
 */
bool Bsp_startCompareMatchTimer( uint8_t channelNum );


/** This method stops the specified Compare Match Timer channel. 

    @param channelNum       CMT channel number. 0=CMT0, 1=CMT1, 2=CMT2, 3=CMT3

    @returns true if successful; else false is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks validates the
             range of arguments 
 */
bool Bsp_stopCompareMatchTimer( uint8_t channelNum );



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
#define Bsp_Api_disableIrqs_MAP()               __builtin_rx_clrpsw('I')

/// Generic API
#define Bsp_Api_enableIrqs_MAP()                __builtin_rx_setpsw('I')

/// Generic API
#define Bsp_Api_pushAndDisableIrqs_MAP()        {__asm volatile ( "PUSHC PSW" );__builtin_rx_clrpsw(16)} 

/// Generic API
#define Bsp_Api_popIrqs_MAP                     __asm volatile ( "POPC PSW" )




#endif  // end header latch
