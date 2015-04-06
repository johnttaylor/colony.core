#ifndef Cpl_Driver_Uart_Hal_h_
#define Cpl_Driver_Uart_Hal_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file 
    
    This file defines a hardware abstraction layer (HAL) for accessing a
    hardware register based UART.

    NOTE: NO 'Initialization' method is provided/defined in this interface -
          this is intentional! The initialization of the baud rate, number of
          start/stop bits, etc. is VERY platform specific - which translate
          to very cumbersome/inefficiencies in trying to make a generic one
          size-fits-all init() method. What does this mean.... well first the
          application is RESPONSIBLE for making sure that the platform specfic
          initialization happens BEFORE any of the Cpl drivers are started.  
          Second, this interface ASSUMES that the required 'Uart Handle' is 
          returned/created (at least conceptually) from this platform specific 
          init() routine.
*/

#include <stdint.h>
#include "colony_map.h"


/*-------------- PUBLIC API ------------------------------------------------*/
/** This data type defines the platform specifc 'handle' of a UART.  The
    'handle' is used to unique identify a specific UART instance.
 */
#define Cpl_Driver_Uart_Hal_T               Cpl_Driver_Uart_Hal_T_MAP



/*-------------- PUBLIC API ------------------------------------------------*/
/** This method places the specified byte in to the transmit data register and
    initiates a transmit sequence.

    Prototype:
        void Cpl_Driver_Uart_Hal_transmitByte( Cpl_Driver_Uart_Hal_T hdl, uint8_t byteToTransmit );
 */
#define  Cpl_Driver_Uart_Hal_transmitByte         Cpl_Driver_Uart_Hal_transmitByte_MAP


/** This method clears the Tx Interrupt request.  This method can be empty/null
    if the loadind the data register clears the IRQ.

    Prototype:
        void Cpl_Driver_Uart_Hal_clrTxIrq( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_clrTxIrq             Cpl_Driver_Uart_Hal_clrTxIrq_MAP


/** This method enables the UART transmitter 

    Prototype:
        void Cpl_Driver_Uart_Hal_enableTx( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_enableTx             Cpl_Driver_Uart_Hal_enableTx_MAP


/** This method disables the UART transmitter 

    Prototype:
        void Cpl_Driver_Uart_Hal_disableTx( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_disableTx            Cpl_Driver_Uart_Hal_disableTx_MAP


/** This method enables the UART Transmit Data register emtpy interrupt 

    Prototype:
        void Cpl_Driver_Uart_Hal_enableTxIrq( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_enableTxIrq          Cpl_Driver_Uart_Hal_enableTxIrq_MAP


/** This method disables the UART Transmit Data register empty interrupt
    Prototype:
        void Cpl_Driver_Uart_Hal_enableTxIrq( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_disableTxIrq         Cpl_Driver_Uart_Hal_disableTxIrq_MAP


/** This method returns true if the Transmit Data empty interrupt is enabled
    Prototype:
        bool Cpl_Driver_Uart_Hal_isTxIrqEnabled( Cpl_Driver_Uart_Hal_T hdl );
 */
#define Cpl_Driver_Uart_Hal_isTxIrqEnabled        Cpl_Driver_Uart_Hal_isTxIrqEnabled_MAP

       
/** This method return non-zero ( Cpl_Driver_Uart_Hal_T hdl, true) if the 'current' UART interrupt
    request is a tx-data-register-empty interrupt.

    Prototype:
        bool Cpl_Driver_Uart_Hal_isTxIrq( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_isTxIrq              Cpl_Driver_Uart_Hal_isTxIrq_MAP


/** This method returns non-zero ( Cpl_Driver_Uart_Hal_T hdl, true) if the UART transmit buffer
    register is empty.  This method is used to 'manually' load the first 
    transmit byte.  

    Prototype
        bool Cpl_Driver_Uart_Hal_isTxPipeEmpty( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_isTxPipeEmpty         Cpl_Driver_Uart_Hal_isTxPipeEmpty_MAP


/** This method is used to reset/prep the UART's transmit engine for
    the first byte transmit condition. 

    Prototype
        void Cpl_Driver_Uart_Hal_resetTxPipe( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_resetTxPipe           Cpl_Driver_Uart_Hal_resetTxPipe_MAP



/*-------------- PUBLIC API ------------------------------------------------*/
/** This method returns the last received byte, independent of any Rx errors

    Prototype:
        uint8_t Cpl_Driver_Uart_Hal_getRxByte( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_getRxByte            Cpl_Driver_Uart_Hal_getRxByte_MAP


/** This method clears the Rx Interrupt request.  This method can be empty/null
    if the reading the received byte clears the IRQ.

    Prototype:
        void Cpl_Driver_Uart_Hal_clrRxIrq( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_clrRxIrq             Cpl_Driver_Uart_Hal_clrRxIrq_MAP


/** This method returns non-zero (true) if any Rx error (framing, overrung, 
    parity, etc.) has occurred on the 'current' byte 

    Prototype:
        uint8_t Cpl_Driver_Uart_Hal_isRxError( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_isRxError            Cpl_Driver_Uart_Hal_isRxError_MAP


/** This method clears ALL Rx errors for the 'current' byte.

    Prototype:
        void Cpl_Driver_Uart_Hal_clrRxErrors( Cpl_Driver_Uart_Hal_T hdl );
*/
#define  Cpl_Driver_Uart_Hal_clrRxErrors          Cpl_Driver_Uart_Hal_clrRxErrors_MAP


/** This method enables the UART receiver 

    Prototype:
        void Cpl_Driver_Uart_Hal_enableRx( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_enableRx             Cpl_Driver_Uart_Hal_enableRx_MAP


/** This method disables the UART receiver 

    Prototype:
        void Cpl_Driver_Uart_Hal_disableRx( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_disableRx            Cpl_Driver_Uart_Hal_disableRx_MAP


/** This method enables the UART Receive data interrupt.

    Prototype:
        void Cpl_Driver_Uart_Hal_enableRxIrq( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_enableRxIrq          Cpl_Driver_Uart_Hal_enableRxIrq_MAP


/** This method disables the UART Receive data interrupt.

    Prototype:
        void Cpl_Driver_Uart_Hal_disableRxIrq( Cpl_Driver_Uart_Hal_T hdl );
 */
#define  Cpl_Driver_Uart_Hal_disableRxIrq         Cpl_Driver_Uart_Hal_disableRxIrq_MAP


/** This method return non-zero (true) if the 'current' UART interrupt
    request is a unread-data-in-the-rx-data-registe interrupt.

    Prototype:
        bool Cpl_Driver_Uart_Hal_isRxIrq( Cpl_Driver_Uart_Hal_T hdl );
 */ 
#define  Cpl_Driver_Uart_Hal_isRxIrq              Cpl_Driver_Uart_Hal_isRxIrq_MAP




/*--------------------------------------------------------------------------*/
#endif  
