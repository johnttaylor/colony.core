/**
  @page DMA_FIFOMode DMA FIFO mode Example
  
  @verbatim
  ******************************************************************************
  * @file    DMA/DMA_FIFOMode/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the DMA FIFO mode example.
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

How to, through the HAL API, use a DMA  to transfer a word data buffer from
Flash memory to embedded SRAM with FIFO mode enabled.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 216 MHz.

DMA2_Stream0 is configured to transfer the contents of a 32-word data 
buffer stored in Flash memory to the reception buffer declared in RAM.

In this example the FIFO mode is enabled and the FIFO threshold is configured
as "FULL". Also an incremental burst transfer of 4 beats is requested. 

The start of transfer is triggered by software. DMA2_Stream0 memory-to-memory
transfer is enabled. Source and destination addresses incrementing is also enabled.
The transfer is started by setting the stream enable bit for DMA2_Stream0.
At the end of the transfer a Transfer Complete interrupt is generated since it
is enabled and the callback function (customized by user) is called.

STM32F767ZI-Nucleo Board's LEDs can be used to monitor the transfer status:
 - LED1 is ON when the transfer is complete (into the Transfer Complete interrupt
   routine).
 - LED2 is ON when there are an error of the transfer (into the Transfer error 
   interrupt routine)
 - LED3 is ON when there are an error in the transfer process or in initialisation 
  (into the Transfer error interrupt routine)
 
It is possible to select a different stream for the DMA transfer
example by modifying defines values in the file main.h.
Note that only DMA2 streams are able to perform Memory-to-Memory transfers.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, DMA, FIFO Mode, Data Transfer, FIFO Threshold, Memory to memory, Stream

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

  - DMA/DMA_FIFOMode/Src/system_stm32f7xx.c    STM32F7xx system clock configuration file
  - DMA/DMA_FIFOMode/Src/stm32f7xx_it.c        Interrupt handlers
  - DMA/DMA_FIFOMode/Src/main.c                Main program
  - DMA/DMA_FIFOMode/Inc/stm32f7xx_hal_conf.h  HAL Configuration file
  - DMA/DMA_FIFOMode/Inc/stm32f7xx_it.h        Interrupt handlers header file
  - DMA/DMA_FIFOMode/Inc/main.h                Main program header file

     
@par Hardware and Software environment

  - This example runs on STM32F767ZI devices.

  - This example has been tested with STMicroelectronics STM32F767ZI-Nucleo 
    evaluation boards and can be easily tailored to any other supported device 
    and development board.    

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

   

 */
