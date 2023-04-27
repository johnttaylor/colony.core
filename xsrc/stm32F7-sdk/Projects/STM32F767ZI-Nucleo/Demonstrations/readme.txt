/**
  @page Demonstrations   Demonstrations STM32F767ZI-Nucleo
 
  @verbatim
  ******************************************************************************
  * @file    Demonstrations/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of STM32F767xx-Nucleo Demo
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

@par Demo Description

The provided demonstration firmware based on STM32Cube helps you to discover STM32 
Cortex-M devices that can be plugged on a NUCLEO-F767ZI board. 

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock
(SYSCLK) to run at 216 MHz.
 
 
Below you find the sequence to discover the demonstration :

 - Check the availability of adafruit 1.8" TFT shield on top of STM32 Nucleo 
   board. This is done by reading the state of IO PF.03 pin (mapped to JoyStick 
   available on adafruit 1.8" TFT shield). If the state of PF.03 is high then 
   the adafruit 1.8" TFT shield is available.
    
   - Adafruit 1.8" TFT shield is not available:
     LED2 is toggling with a first frequency equal to ~1Hz.
     Pressing User button lets LED2 toggling with a second frequency equal to ~5Hz.
     Pressing User button again, changes LED2 toggling frequency to ~10Hz.
     This is done in an infinite loop.      

   - Adafruit 1.8" TFT shield is available:
     A menu will be displayed on TFT. Follow instructions below:
     
     - Joystick DOWN to continue menu display
     - Choose the desired display mode: Press Joystick DOWN for automatic mode 
       or Joystick UP for manual mode.
       
       - Manual Mode selected:
         Images available on SD Card, are displayed by pressing Joystick RIGHT 
         to display next image or Joystick LEFT to display previous one. 
         Pressing long (~1s) Joystick SEL, switches display mode to automatic one.
       
       - Automatic Mode selected:
         Images available on SD Card are displayed sequentially in a forever loop.


It is worth noting that the application manages some errors occurred during the 
access to uSD card to parse bmp images:

 - If SD Card is not FAT formatted, a message will be displayed on TFT. 
   In this case, format the SD card and put into its root directory the .bmp 
   files available within the "BMP_128x160" folder under "Utilities/Media/Pictures"

 - If the content of the SD card is other than a bitmap file, a message will 
   be displayed on TFT mentioning that it is not supported.
   In this case, put into the SD card's root directory the .bmp
   files available within the "BMP_128x160" folder under "Utilities/Media/Pictures" 
   which are respecting the following criteria:
    o Dimensions: 128x160
    o Width: 128 pixels
    o Height: 160 pixels
    o Bit depth: 16
    o Item type: BMP file
    o The name of the bmp image file must not exceed 11 characters (including
      .bmp extension).


@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. 
      This implies that if HAL_Delay() is called from a peripheral ISR process, 
      then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to
      1 millisecond to have correct HAL operation.

@note The clock setting is configured to have the max product performance (max clock frequency) 
      so not optimized in term of power consumption.

@par Keywords

Demonstrations, Adafruit, Shield, TFT, Joystick, SD Card, Display, 

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
 
  - Demonstrations/Inc/stm32f7xx_hal_conf.h    HAL configuration file
  - Demonstrations/Inc/stm32f7xx_it.h          Interrupt handlers header file
  - Demonstrations/Inc/main.h                  Header for main.c module
  - Demonstrations/Inc/fatfs_storage.h         Header for fatfs_storage.c module
  - Demonstrations/Inc/ffconf.h                FAT file system module configuration file 
  - Demonstrations/Inc/sd_diskio.h             SD diskio header file  
  - Demonstrations/Src/stm32f7xx_it.c          Interrupt handlers
  - Demonstrations/Src/main.c                  Main program
  - Demonstrations/Src/fatfs_storage.c         Storage (FatFs) driver
  - Demonstrations/Src/system_stm32f7xx.c      STM32F7xx system source file
  - Demonstrations/Src/sd_diskio.c             SD diskio source file


@par Hardware and Software environment

  - This demonstration runs on STM32F767ZI devices.
    
  - This demonstration has been tested with STMicroelectronics NUCLEO-F767ZI Rev.B 
    board and can be easily tailored to any other supported device 
    and development board.
    
  - A SDSC microSD card (capacity up to 8GB), must contain the .bmp files
    available within the "BMP_128x160" folder under "Utilities/Media/Pictures" 
    
  - Adafruit 1.8" TFT shield must be connected on CN7,CN10, CN8 and CN9 Arduino connectors, 
    for more details please refer to UM1726.

For more details about the adafruit 1.8" TFT shield, please visit: 
http://www.adafruit.com/blog/2012/04/26/new-product-adafruit-1-8-18-bit-color-tft-shield-wmicrosd-and-joystick/


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the demonstration


 */
 
