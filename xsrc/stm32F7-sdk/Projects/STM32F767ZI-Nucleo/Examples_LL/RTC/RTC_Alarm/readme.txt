/**
  @page RTC_Alarm RTC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_Alarm/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RTC example.
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

Configuration of the RTC LL API to configure and generate an alarm using the RTC peripheral. The peripheral initialization 
uses LL unitary service functions for optimization purposes (performance and size).

In this example, after start-up, SYSCLK is configured to the max frequency using the PLL with
HSE as clock source.

The RTC peripheral configuration is ensured by the Configure_RTC() function 
(configure of the needed RTC resources according to the used hardware CLOCK, 
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

@note LSE oscillator clock is used as RTC clock source by default.
      The user can use also LSI as RTC clock source. The user uncomment the adequate 
      line on the main.c file.
      @code
        #define RTC_CLOCK_SOURCE_LSE  
        /* #define RTC_CLOCK_SOURCE_LSI */
      @endcode
      LSI oscillator clock is delivered by a 32 kHz RC.
      LSE (when available on board) is delivered by a 32.768 kHz crystal.

Configure_RTC_Alarm function is then called to initialize the 
time, date and alarm.

In this example, the Time is set to 11:59:55 PM and the Alarm must be generated after 
30 seconds on 12:00:25 AM.

The current time and date are updated and displayed on the debugger in aShowTime 
and aShowDate variables (watch or live watch).

- LED1 is turned ON when the RTC Alarm is generated correctly.
- LED1 is toggling : This indicates that the system generates an error.

@par Keywords

System, RTC, Alarm, wakeup timer, Backup domain, Counter, LSE, LSI

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

  - RTC/RTC_Alarm/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - RTC/RTC_Alarm/Inc/main.h                  Header for main.c module
  - RTC/RTC_Alarm/Inc/stm32_assert.h          Template file to include assert_failed function
  - RTC/RTC_Alarm/Src/stm32f7xx_it.c          Interrupt handlers
  - RTC/RTC_Alarm/Src/main.c                  Main program
  - RTC/RTC_Alarm/Src/system_stm32f7xx.c      STM32F7xx system source file


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
