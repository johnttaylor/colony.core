/**
  @page TIM_PWMInput TIM PWM Input example
  
  @verbatim
  ******************************************************************************
  * @file    TIM/TIM_PWMInput/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the TIM PWM_Input example.
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

Use of the TIM peripheral to measure an external signal frequency and 
duty cycle.

The TIM3CLK frequency is set to SystemCoreClock/2 (Hz), the Prescaler is 0 so the 
counter clock is SystemCoreClock/2 (Hz).
SystemCoreClock is set to 216 MHz for STM32F767xx Devices.

TIM3 is configured in PWM Input Mode: the external signal is connected to 
TIM3 Channel2 used as input pin.
To measure the frequency and the duty cycle, we use the TIM3 CC2 interrupt request,
so in the TIM3_IRQHandler routine, the frequency and the duty cycle of the external 
signal are computed.

"uwFrequency" variable contains the external signal frequency:
TIM3 counter clock = SystemCoreClock/2,
uwFrequency = TIM3 counter clock / TIM3_CCR2 in Hz, 

"uwDutyCycle" variable contains the external signal duty cycle:
uwDutyCycle = (TIM3_CCR1*100)/(TIM3_CCR2) in %.

The minimum frequency value to measure is (TIM3 counter clock / CCR MAX)
                                         = (216 MHz/2)/ 65535
                                         = 1647 Hz

TIM2 can be used to generate the external signal in case a function generator 
is not available. TIM2 is configured in PWM Output Mode to produce a square wave on PA.15.
Frequency and duty cycles can be changed by pressing the User Button (PC13).
Six combinatins are available (see thables aFrequency[] and aDutyCycle[]).

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Timers, Input, signals, PWM, External signal, Frequency, Duty cycle, Measure

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

  - TIM/TIM_PWMInput/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - TIM/TIM_PWMInput/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - TIM/TIM_PWMInput/Inc/main.h                  Header for main.c module  
  - TIM/TIM_PWMInput/Src/stm32f7xx_it.c          Interrupt handlers
  - TIM/TIM_PWMInput/Src/main.c                  Main program
  - TIM/TIM_PWMInput/Src/stm32f7xx_hal_msp.c     HAL MSP file
  - TIM/TIM_PWMInput/Src/system_stm32f7xx.c      STM32F7xx system source file


@par Hardware and Software environment

  - This example runs on STM32F767xx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-F767ZI 
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-F767ZI Set-up
    - You can either Connect the external signal to measure to the TIM3 CH2 pin (PA.07) (pin 14 in CN7 connector).
    - Or connect TIM3 CH1 pin PA.15 (pin 9 in CN7 connector) to the TIM3 CH2 pin (PA.07) instead of an external signal.

	
@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
