/**
  @page USART_SyncCommunication_FullDuplex_IT USART Synchronous Transmitter/Receiver example (IT mode)
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_SyncCommunication_FullDuplex_IT/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_SyncCommunication_FullDuplex_IT example.
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

Configuration of GPIO, USART, DMA and SPI peripherals to transmit 
bytes between a USART and an SPI (in slave mode) in Interrupt mode. This example is based on the STM32F7xx USART LL API 
(the SPI uses the DMA to receive/transmit characters sent from/received by the USART). The peripheral 
initialization uses LL unitary service functions for optimization purposes (performance and size).

This example works with only one NUCLEO-F767ZI.

USART1 Peripheral is acts as Master using IT.
SPI1 Peripheral is configured in Slave mode using DMA.
GPIO associated to User push-button is linked with EXTI. 

Example execution:

LED1 is blinking Fast (200ms) and wait User push-button action.
Press User push-button on BOARD start a Full-Duplex communication from USART using IT.
On USART1 side, Clock will be generated on SCK line, Transmission done on TX, reception on RX.
On SPI1 side, reception is done through the MOSI Line, transmission on MISO line.

At end of transmission, both received buffers are compared to expected ones.
In case of both transfers successfully completed, LED1 is turned on.
In case of errors, LED1 is blinking Slowly (1s).

@par Keywords

Connectivity, USART, Synchronous mode, RS-232, baud rate, Interrupt, SPI, Reception, Transmission, Full Duplex

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

  - USART/USART_SyncCommunication_FullDuplex_IT/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - USART/USART_SyncCommunication_FullDuplex_IT/Inc/main.h                  Header for main.c module
  - USART/USART_SyncCommunication_FullDuplex_IT/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_SyncCommunication_FullDuplex_IT/Src/stm32f7xx_it.c          Interrupt handlers
  - USART/USART_SyncCommunication_FullDuplex_IT/Src/main.c                  Main program
  - USART/USART_SyncCommunication_FullDuplex_IT/Src/system_stm32f7xx.c      STM32F7xx system source file

@par Hardware and Software environment

  - This example runs on STM32F767xx devices.

  - This example has been tested with NUCLEO-F767ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-F767ZI Set-up
    - Connect USART1 SCK PA.08 to Slave SPI1 SCK PB.03
    - Connect USART1 TX PA.09 to Slave SPI1 MOSI PB.05
    - Connect USART1 RX PA.10 to Slave SPI1 MISO PB.04

Board connector:
  PA.08 :   connected to pin 23 of CN12 connector
  PA.09 :   connected to pin 21 of CN12 connector
  PA.10 :   connected to pin 33 of CN12 connector
  PB.03 :   connected to pin 31 of CN12 connector
  PB.05 :   connected to pin 29 of CN12 connector
  PB.04 :   connected to pin 27 of CN12 connector

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Launch the program. Press on User push button on board to initiate data transfers.


 */
