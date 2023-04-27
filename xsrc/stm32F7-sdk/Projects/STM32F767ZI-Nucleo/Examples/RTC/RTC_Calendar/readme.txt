/**
  @page RTC_Calendar RTC Calendar Example
  
  @verbatim
  ******************************************************************************
  * @file    RTC/RTC_Calendar/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RTC Calendar example.
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

Configuration of the calendar using the RTC HAL API.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 216 MHz.

The RTC peripheral configuration is ensured by the HAL_RTC_Init() function.
This later is calling the HAL_RTC_MspInit()function which core is implementing
the configuration of the needed RTC resources according to the used hardware (CLOCK, 
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

LSE oscillator clock is used as RTC clock source. 

HAL_RTC_SetTime()and HAL_RTC_SetDate() functions are then called to initialize the 
time and the date.

A key value is written in backup data register 1 to indicate if the RTC is already configured.  
The program behaves as follows:

1. After startup the program checks the backup data register 1 value:
    - BKP_DR1 value not correct: (RTC_BKP_DR1 value is not correct or has not yet
      been programmed when the program is executed for the first time) the RTC is
      configured.
    
    - BKP_DR1 value correct: this means that the RTC is configured and the time
      and date are displayed on Debugger.
2. When a reset (except power on reset) occurs the BKP domain is not reset and the RTC 
   configuration is not lost. LED1 is ON.
   
3. When power on reset occurs:
    - The BKP domain is reset and the RTC configuration is lost.
    Note: During first debugger re-run, the power on reset already occurred just before 
    (at switch ON) then RTC configuration is not lost and LED2 is ON.

LED1 is turned ON when the RTC configuration is done correctly.

The current time and date are updated and displayed on the debugger in aShowTime 
and aShowDate variables.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, RTC, Calendar, Backup Domain, Reset

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

  - RTC/RTC_Calendar/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - RTC/RTC_Calendar/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - RTC/RTC_Calendar/Inc/main.h                  Header for main.c module  
  - RTC/RTC_Calendar/Src/stm32f7xx_it.c          Interrupt handlers
  - RTC/RTC_Calendar/Src/main.c                  Main program
  - RTC/RTC_Calendar/Src/stm32f7xx_hal_msp.c     HAL MSP module
  - RTC/RTC_Calendar/Src/system_stm32f7xx.c      STM32F7xx system source file


@par Hardware and Software environment

  - This example runs on STM32F767ZI devices.
  - This example has been tested with STMicroelectronics STM32F767ZI-Nucleo
    board and can be easily tailored to any other supported device and 
    development board. 

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 */
