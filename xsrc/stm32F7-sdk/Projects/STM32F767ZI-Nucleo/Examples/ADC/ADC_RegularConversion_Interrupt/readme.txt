/**
  @page ADC_RegularConversion_Interrupt conversion using interrupt

  @verbatim
  ******************************************************************************
  * @file    ADC/ADC_RegularConversion_Interrupt/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC RegularConversion interrupt example.
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

How to use the ADC in interrupt mode to convert data through the HAL API.

The ADC1 is configured to convert continuously ADC_CHANNEL_10.

Each time an end of conversion occurs an interrupt is generated and the converted 
data of ADC1 DR register is affected to the uhADCxConvertedValue variable in the 
ADC conversion complete call back function.

In this example, the system clock is 216MHz, APB2 = 108MHz and ADC clock = APB2/4. 
Since ADC1 clock is 27 MHz and sampling time is set to 3 cycles, the conversion 
time to 12bit data is 12 cycles so the total conversion time is (12+3)/27= 0.55us(1.8Msps).

User can vary the ADC_CHANNEL_10 voltage by applying an input voltage on pin PC.00 (pin3 connector 9) (e.g. using function generator).

The converted value is monitored through debugger using the uhADCxConvertedValue variable

STM32 Nucleo board's LEDs can be used to monitor the transfer status:
 - LED3 is ON when there is an initialization error.

@par Keywords

Analog, ADC, Analog to Digital Converter, Regular Conversion, Interrupt, Continuous Conversion

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

  - ADC/ADC_RegularConversion_Interrupt/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - ADC/ADC_RegularConversion_Interrupt/Inc/stm32f7xx_it.h          DMA interrupt handlers header file
  - ADC/ADC_RegularConversion_Interrupt/Inc/main.h                  Header for main.c module  
  - ADC/ADC_RegularConversion_Interrupt/Src/stm32f7xx_it.c          DMA interrupt handlers
  - ADC/ADC_RegularConversion_Interrupt/Src/main.c                  Main program
  - ADC/ADC_RegularConversion_Interrupt/Src/stm32f7xx_hal_msp.c     HAL MSP file 
  - ADC/ADC_RegularConversion_Interrupt/Src/system_stm32f7xx.c      STM32F7xx system source file

@par Hardware and Software environment 

  - This example runs on STM32F767ZI devices.
  
  - This example has been tested with STM32F767ZI-Nucleo_144 board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 */
