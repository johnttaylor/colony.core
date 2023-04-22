/**
  @page ADC_MultimodeDualInterleaved ADC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/ADC/ADC_MultimodeDualInterleaved/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC_MultimodeDualInterleaved example.
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

How to use several ADC peripherals in multimode and interleaved mode.

This example is based on the STM32F7xx ADC LL API; 
peripheral initialization done using LL unitary services functions
for optimization purpose (performance and size).

Description of multimode interleaved:
ADC master instance synchronizes and manages ADC slave instance.
Multimode interleaved combines ADC instances to convert
the same channel and increase the overall ADC conversion rate.
This example configures the ADC to perform conversions at the 
maximum ADC conversion rate possible (with a sampling time  
corresponding to ADC resolution 12 bits).


Example configuration:
ADC1 and ADC2 instances (master and slave) are configured in continuous conversion mode,
from SW trigger (in order to reach more simply the maximum conversion rate possible,
but trigger from timer is also possible).
Both ADC instances are configured to convert a single channel (1 channel from a GPIO),
this channel is common to ADC1 and ADC2.
DMA is configured to transfer conversion data in an array, in circular mode.
Note: One DMA channel is used (the one of ADC master) to transfer data from
      both ADC instances (master and slave).
      Usage of two DMA channels (one for ADC master, one for ADC slave) is also
      possible: this is the recommended configuration in case of high ADC
      conversions rates and applications using other DMA channels intensively.

Example execution:
From the first press on push button, the ADC converts the selected channel
continuously.
DMA transfers conversion data to the array, transfer complete interruption occurs.
Results array is updated indefinitely (DMA in circular mode).
LED1 is turned on when the DMA transfer is completed (results array full)
and turned off at next DMA half-transfer (result array first half updated).

Note: ADC is converting at high frequency, therefore LED1 is toggling at high frequency
      and will appear as dimming.
      Use an oscilloscope probe on LED1 to watch DMA transfers frequency.

Note: A file is provide with this example to detail the ADC conversions timings.

For debug: variables to monitor with debugger watch window:
 - "aADCxADCyMultimodeDualConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE]": ADC multimode dual conversion data: ADC master and ADC slave conversion data are concatenated in a registers of 32 bits.
 - "aADCxMultimodeDualMasterConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE]": For the purpose of this example, dispatch multimode dual conversion data into array corresponding to ADC master conversion data.
 - "aADCyMultimodeDualSlaveConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE]": For the purpose of this example, dispatch multimode dual conversion data into array corresponding to ADC slave conversion data.
 - "ubDmaTransferStatus": status of DMA transfer of ADC group regular conversions

Connection needed:
None.
Note: Optionally, a voltage can be supplied to the analog input pin (cf pin below),
      between 0V and Vdda=3.3V, to perform a ADC conversion on a determined
      voltage level.
      Otherwise, this pin can be let floating (in this case ADC conversion data
      will be undetermined).

Other peripherals used:
  1 GPIO for push button
  1 GPIO for LED1
  1 GPIO for analog input: PA.04 (connector CN7 pin 17)
  DMA

@par Keywords

Analog, ADC, Analog to Digital, Multi Mode, Interleaved, DMA, Dual, Measurement

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

  - ADC/ADC_MultimodeDualInterleaved/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - ADC/ADC_MultimodeDualInterleaved/Inc/main.h                  Header for main.c module
  - ADC/ADC_MultimodeDualInterleaved/Inc/stm32_assert.h          Template file to include assert_failed function
  - ADC/ADC_MultimodeDualInterleaved/Src/stm32f7xx_it.c          Interrupt handlers
  - ADC/ADC_MultimodeDualInterleaved/Src/main.c                  Main program
  - ADC/ADC_MultimodeDualInterleaved/Src/system_stm32f7xx.c      STM32F7xx system source file


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
