/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2017  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Bsp/Api.h"
#include "stm32f4xx_hal.h"
#include "Bsp/ST/NUCLEO-F413ZH/alpha1/MX/Core/Inc/usart.h"
#include "Bsp/ST/NUCLEO-F413ZH/alpha1/MX/Core/Inc/gpio.h"
#include <ios>

// Map printf to the UART3
extern "C" {
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
}


#define SECT_   "bsp"




///////////////////////////////////////////
void Bsp_Api_initialize( void )
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();


    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART3_UART_Init();

      // Configure the LEDs as output pins 
      //pinMode( OPTION_BSP_DEBUG_LED1_INDEX, OUTPUT );
}

