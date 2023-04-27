/**
  @page DMA2D_MemToMemWithRBSwap DMA2D : DMA2D Memory to Memory with R&B swap example (HAL/LL mixed usage example)
  
  @verbatim
  ******************************************************************************
  * @file    Examples_MIX/DMA2D/DMA2D_MemToMemWithRBSwap/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the DMA2D_MemToMemWithRBSwap example.
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

How to configure DMA2D peripheral in Memory-to-memory transfer mode with pixel
format conversion and image blending, then display the result on LCD. The DMA2D
LL APIs are used for performance improvement.

  The source buffer in format ARGB8888 and size 128x160 located in Flash is copied
  in Memory to Memory mode with Pixel Format Conversion to create three
  different images:
  - the first one is an artificially distorted image (converted in ARGB1555 but read as RGB565)
  - the second one is converted to RGB565 but Red and Blue are swapped for illustration purposes
  - the third one is converted to RGB565 with the correct color setting

  These three images are alternatively shown on the LCD, blended two by two so that each
  image smoothly gives way to the next one.
  The last image finally fades away to let a white screen only.
  Then the same process is repeated in an infinite loop.


  In this basic example, the goal is to show how to set some of the DMA2D IP
  parameters thru DMAD LL APIs:
  - input and output image format, alpha value
  - origin and destination buffers addresses
  - foreground and background addresses
  - transfer size
  - pixel format conversion, blending
  - DMA2D interruptions enabling
  - DMA2D transfer start 

  NUCLEO-F767ZI board's LEDs can be used to monitor the transfer status:
 - LED1 is ON when the DMA2D transfer is complete.
 - LED2 is ON when there is a DMA2D transfer error.
 - LED3 is ON if the adafruit shield is not detected.


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Display, Graphic, DMA2D, Memory to memory, LCD, ARGB8888, Red Blue swap, PFC

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

  - DMA2D/DMA2D_MemToMemWithRBSwap/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - DMA2D/DMA2D_MemToMemWithRBSwap/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - DMA2D/DMA2D_MemToMemWithRBSwap/Inc/main.h                  Header for main.c module
  - DMA2D/DMA2D_MemToMemWithRBSwap/Inc/ARGB8888_128x160.h      ARGB8888 Image binary
  - DMA2D/DMA2D_MemToMemWithRBSwap/Src/stm32f7xx_it.c          Interrupt handlers
  - DMA2D/DMA2D_MemToMemWithRBSwap/Src/main.c                  Main program
  - DMA2D/DMA2D_MemToMemWithRBSwap/Src/stm32f7xx_hal_msp.c     HAL MSP module 
  - DMA2D/DMA2D_MemToMemWithRBSwap/Src/system_stm32f7xx.c      STM32F7xx system source file


@par Hardware and Software environment

  - This example runs on STM32F767xx devices.
  
  - This example has been tested with STMicroelectronics NUCLEO-F767ZI 
    board and can be easily tailored to any other supported device 
    and development board. 

  - Adafruit 1.8" TFT shield must be connected on CN7, CN8, CN9 and CN10 Arduino connectors,
    for more details please refer to UM1726.

For more details about the adafruit 1.8" TFT shield, please visit: 
http://www.adafruit.com/blog/2012/04/26/new-product-adafruit-1-8-18-bit-color-tft-shield-wmicrosd-and-joystick/

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
