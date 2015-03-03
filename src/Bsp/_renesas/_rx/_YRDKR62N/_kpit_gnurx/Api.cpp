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

#include "Api.h"

////////////////////////////////////////////////////////

/// Private: LED Constants
#define  LED4_                  PORTD.DR.BIT.B5
#define  LED5_                  PORTE.DR.BIT.B3
#define  LED6_                  PORTD.DR.BIT.B2
#define  LED7_                  PORTE.DR.BIT.B0
#define  LED8_                  PORTD.DR.BIT.B4
#define  LED9_                  PORTE.DR.BIT.B2
#define  LED10_                 PORTD.DR.BIT.B1
#define  LED11_                 PORTD.DR.BIT.B7
#define  LED12_                 PORTD.DR.BIT.B3
#define  LED13_                 PORTE.DR.BIT.B1
#define  LED14_                 PORTD.DR.BIT.B0
#define  LED15_                 PORTD.DR.BIT.B6

// Private: Push button Switch Constants
#define  PUSH_BUTTON_SW1_       PORT4.PORT.BIT.B0
#define  PUSH_BUTTON_SW2_       PORT4.PORT.BIT.B1
#define  PUSH_BUTTON_SW3_       PORT4.PORT.BIT.B2


///////////////////////////////////////////
void Bsp_Api_initialize( void )
    {
	// Set clock speeds 
    uint32_t sckcr = 0;
	sckcr += (BSP_SYSCLK_ICLK_MUL==8) ? (0ul << 24) : (BSP_SYSCLK_ICLK_MUL==4) ? (1ul << 24) : (BSP_SYSCLK_ICLK_MUL==2) ? (2ul << 24) : (3ul << 24);
	sckcr += (BSP_SYSCLK_BCLK_MUL==8) ? (0ul << 16) : (BSP_SYSCLK_BCLK_MUL==4) ? (1ul << 16) : (BSP_SYSCLK_BCLK_MUL==2) ? (2ul << 16) : (3ul << 16);
	sckcr += (BSP_SYSCLK_PCLK_MUL==8) ? (0ul <<  8) : (BSP_SYSCLK_PCLK_MUL==4) ? (1ul <<  8) : (BSP_SYSCLK_PCLK_MUL==2) ? (2ul <<  8) : (3ul <<  8);
	SYSTEM.SCKCR.LONG = sckcr;



    // Configure push button Switches (SW1, SW2, SW3) for polled input
    PORT4.DDR.BIT.B0 = 0;
    PORT4.DDR.BIT.B1 = 0;
    PORT4.DDR.BIT.B2 = 0;
    PORT4.ICR.BIT.B0 = 1;
    PORT4.ICR.BIT.B1 = 1;
    PORT4.ICR.BIT.B2 = 1;


    // Configure 'Circle' LEDs (set port pins as Outputs)
    PORTD.DDR.BIT.B0 = 1;      
    PORTD.DDR.BIT.B1 = 1;
    PORTD.DDR.BIT.B2 = 1;
    PORTD.DDR.BIT.B3 = 1;
    PORTD.DDR.BIT.B4 = 1;
    PORTD.DDR.BIT.B5 = 1;
    PORTD.DDR.BIT.B6 = 1;
    PORTD.DDR.BIT.B7 = 1;
    PORTE.DDR.BIT.B0 = 1;
    PORTE.DDR.BIT.B1 = 1;
    PORTE.DDR.BIT.B2 = 1;
    PORTE.DDR.BIT.B3 = 1;

    // Default all of the circle LEDs to off
    Bsp_setCircleLED( 0, false );                                               
    }


///////////////////////////////////////////
bool Bsp_initCompareMatchTimer( uint8_t channelNum, uint32_t irqRateInHZ, uint8_t pclkDivider, uint8_t irqPriority )
    {
    // Validate the IRQ Priority
    if ( irqPriority < 1 || irqPriority > 15 )
        {
        return false;
        }


    // Convert pclk divider to a numeric value
    uint16_t divider = 0;
    switch( pclkDivider )
        {
        default:
            return false;
            // this is NOT a fall through case!

        case 0:
            divider = 8;
            break;

        case 1:
            divider = 32;
            break;

        case 2:
            divider = 128;
            break;

        case 3:
            divider = 512;
            break;
        }


    // Calculate the Match value. Note: The IRQ generation is DELAYED 1 count AFTER the match
	uint16_t match = (uint16_t) ((BSP_SYSCLK_PCLK_FREQUENCY / divider / irqRateInHZ) -1);   


    // Configure the Timer Channel
    switch( channelNum )
        {
        // This is an error! 
        default:
            return false;
            // this is NOT a fall through case!

        case 0:
	        MSTP( CMT0 )       = 0;             // Enable the Channel
	        CMT0.CMCR.BIT.CMIE = 1;             // Enable IRQ on match
	        CMT0.CMCOR         = match;         // Set match value.
	        CMT0.CMCR.BIT.CKS  = 0;             // Set the divider       
	        IPR( CMT0,CMI0 )   = irqPriority;   // Set IRQ priority;
	        IEN( CMT0,CMI0 )   = 1;             // Enable the timer interrupt. Note: Until I start the timer - this will-not/cannnot fire
            break;

        case 1:
	        MSTP( CMT1 )       = 0;            
	        CMT1.CMCR.BIT.CMIE = 1;            
	        CMT1.CMCOR         = match;        
	        CMT1.CMCR.BIT.CKS  = 0;            
	        IPR( CMT1,CMI1 )   = irqPriority;   
	        IEN( CMT1,CMI1 )   = 1;            
            break;

        case 2:
	        MSTP( CMT2 )       = 0;            
	        CMT2.CMCR.BIT.CMIE = 1;            
	        CMT2.CMCOR         = match;        
	        CMT2.CMCR.BIT.CKS  = 0;            
	        IPR( CMT2,CMI2 )   = irqPriority;   
	        IEN( CMT2,CMI2 )   = 1;            
            break;

        case 3:
	        MSTP( CMT3 )       = 0;            
	        CMT3.CMCR.BIT.CMIE = 1;            
	        CMT3.CMCOR         = match;        
	        CMT3.CMCR.BIT.CKS  = 0;            
	        IPR( CMT3,CMI3 )   = irqPriority;   
	        IEN( CMT3,CMI3 )   = 1;            
            break;
        }

    return true;
    }
     
        
bool Bsp_startCompareMatchTimer( uint8_t channelNum )
    {
    // Configure the Timer Channel
    switch( channelNum )
        {
        // This is an error! 
        default:
            return false;
            // this is NOT a fall through case!

        case 0:
	        CMT.CMSTR0.BIT.STR0 = 1; 
            break;

        case 1:
	        CMT.CMSTR0.BIT.STR1 = 1; 
            break;

        case 2:
	        CMT.CMSTR1.BIT.STR2 = 1; 
            break;

        case 3:
	        CMT.CMSTR1.BIT.STR3 = 1; 
            break;
        }

    return true;
    }


bool Bsp_stopCompareMatchTimer( uint8_t channelNum )
    {
    // Configure the Timer Channel
    switch( channelNum )
        {
        // This is an error! 
        default:
            return false;
            // this is NOT a fall through case!

        case 0:
	        CMT.CMSTR0.BIT.STR0 = 0; // Stop the timer
	        IEN( CMT0,CMI0 )    = 0; // Disable the timer interrupt
	        IR( CMT0,CMI0 )     = 0; // Clear any pending/leftover IR request flag
            break;

        case 1:
	        CMT.CMSTR0.BIT.STR1 = 0; 
	        IEN( CMT1,CMI1 )    = 0; 
	        IR( CMT1,CMI1 )     = 0; 
            break;

        case 2:
	        CMT.CMSTR1.BIT.STR2 = 0; 
	        IEN( CMT2,CMI2 )    = 0; 
	        IR( CMT2,CMI2 )     = 0; 
            break;

        case 3:
	        CMT.CMSTR1.BIT.STR3 = 0; 
	        IEN( CMT3,CMI3 )    = 0; 
	        IR( CMT3,CMI3 )     = 0; 
            break;
        }

    return true;
    }


///////////////////////////////////////////
void Bsp_setCircleLED( uint8_t ledIndex, bool setToOn )
    {
    switch( ledIndex ) 
        {
        case 0:
            
            LED4_  = !setToOn;
            LED5_  = !setToOn;
            LED6_  = !setToOn;
            LED7_  = !setToOn;
            LED8_  = !setToOn;
            LED9_  = !setToOn;
            LED10_ = !setToOn;
            LED11_ = !setToOn;
            LED12_ = !setToOn;
            LED13_ = !setToOn;
            LED14_ = !setToOn;
            LED15_ = !setToOn;
            break;

        case 4:
            LED4_ = !setToOn;
            break;

        case 5:
            LED5_ = !setToOn;
            break;

        case 6:
            LED6_ = !setToOn;
            break;

        case 7:
            LED7_ = !setToOn;
            break;

        case 8:
            LED8_ = !setToOn;
            break;

        case 9:
            LED9_ = !setToOn;
            break;

        case 10:
            LED10_ = !setToOn;
            break;

        case 11:
            LED11_ = !setToOn;
            break;

        case 12:
            LED12_ = !setToOn;
            break;

        case 13:
            LED13_ = !setToOn;
            break;

        case 14:
            LED14_ = !setToOn;
            break;

        case 15:
            LED15_ = !setToOn;
            break;

        default:
            break;
        }
    }

void Bsp_toggleCircleLED( uint8_t ledIndex )
    {
    switch( ledIndex ) 
        {
        case 0:
            
            LED4_  ^= 1;
            LED5_  ^= 1;
            LED6_  ^= 1;
            LED7_  ^= 1;
            LED8_  ^= 1;
            LED9_  ^= 1;
            LED10_ ^= 1;
            LED11_ ^= 1;
            LED12_ ^= 1;
            LED13_ ^= 1;
            LED14_ ^= 1;
            LED15_ ^= 1;
            break;

        case 4:
            LED4_ ^= 1;
            break;

        case 5:
            LED5_ ^= 1;
            break;

        case 6:
            LED6_ ^= 1;
            break;

        case 7:
            LED7_ ^= 1;
            break;

        case 8:
            LED8_ ^= 1;
            break;

        case 9:
            LED9_ ^= 1;
            break;

        case 10:
            LED10_ ^= 1;
            break;

        case 11:
            LED11_ ^= 1;
            break;

        case 12:
            LED12_ ^= 1;
            break;

        case 13:
            LED13_ ^= 1;
            break;

        case 14:
            LED14_ ^= 1;
            break;

        case 15:
            LED15_ ^= 1;
            break;

        default:
            break;
        }
    }


///////////////////////////////////////////
bool Bsp_isButtonPressed( uint8_t buttonIndex )
    {
    switch( buttonIndex )
        {
        case 1:
            return !PUSH_BUTTON_SW1_; 
            break;
        
        case 2:
            return !PUSH_BUTTON_SW2_; 
            break;
        
        case 3:
            return !PUSH_BUTTON_SW3_; 
            break;

        default:
            break;
        }
   
    return false;
    }

