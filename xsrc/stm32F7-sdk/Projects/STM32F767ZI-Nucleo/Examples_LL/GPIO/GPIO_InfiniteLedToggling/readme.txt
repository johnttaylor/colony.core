/**
  @page GPIO_InfiniteLedToggling GPIO example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/GPIO/GPIO_InfiniteLedToggling/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the GPIO example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

How to configure and use GPIOs to toggle the on-board user LEDs 
every 250 ms. This example is based on the STM32F7xx LL API. The peripheral 
is initialized with LL unitary service functions to optimize 
for performance and size.


PB.0 IO (configured in output pushpull mode) toggles in a forever loop.
On NUCLEO-F767ZI board this IO is connected to LED1.

In this example, HCLK is configured at 216 MHz.

@par Keywords

System, GPIO, Output, Alternate function, Push-pull, Toggle

@Note�If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),that is shared between several processors,
 �����then it is highly recommended to enable the CPU cache and maintain its coherence at application level.
������The address and the size of cacheable buffers (shared between CPU and other masters)  must be properly updated to be aligned to cache line size (32 bytes).

@Note It is recommended to enable the cache and maintain its coherence, but depending on the use case
����� It is also possible to configure the MPU as "Write through", to guarantee the write access coherence.
������In that case, the MPU must be configured as Cacheable/Bufferable/Not Shareable.
������Even though the user must manage the cache coherence for read accesses.
������Please refer to the AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�
������Please refer to the AN4839 �Level 1 cache on STM32F7 Series�

@par Directory contents 

  - GPIO/GPIO_InfiniteLedToggling/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - GPIO/GPIO_InfiniteLedToggling/Inc/main.h                  Header for main.c module
  - GPIO/GPIO_InfiniteLedToggling/Inc/stm32_assert.h          Template file to include assert_failed function
  - GPIO/GPIO_InfiniteLedToggling/Src/stm32f7xx_it.c          Interrupt handlers
  - GPIO/GPIO_InfiniteLedToggling/Src/main.c                  Main program
  - GPIO/GPIO_InfiniteLedToggling/Src/system_stm32f7xx.c      STM32F7xx system source file


@par Hardware and Software environment

  - This example runs on STM32F767xx devices.
    
  - This example has been tested with NUCLEO-F767ZI board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
