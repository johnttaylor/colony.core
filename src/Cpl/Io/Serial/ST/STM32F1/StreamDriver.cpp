/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "StreamDriver.h"
#include "Cpl/System/FatalError.h"

/// 
using namespace Cpl::Io::Serial::ST::STM32F1;


StreamDriver::HalMapping_T Cpl::Io::Serial::ST::STM32F1::StreamDriver::m_mappings[OPTION_CPL_IO_SERIAL_ST_SMT32F1_MAX_UARTS] ={ 0, };


////////////////////////
StreamDriver::StreamDriver( Cpl::Container::RingBuffer<uint8_t>& txBuffer,
                            Cpl::Container::RingBuffer<uint8_t>& rxBuffer ) noexcept
    : m_uartHdl( nullptr )
    , m_txWaiterPtr( nullptr )
    , m_rxWaiterPtr( nullptr )
    , m_txBuffer( txBuffer )
    , m_rxBuffer( rxBuffer )
{
    // generating 1st half of the HAL/Driver mapping (basically it 'allocates' a slot in the look-up table for the instance)
    bool mapped = false;
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_SMT32F1_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].driver == nullptr )
        {
            m_mappings[idx].driver = this;
            mapped                 = true;
            break;
        }
    }
    if ( !mapped )
    {
        Cpl::System::FatalError::logf( "Cpl::Io::Serial::ST::STM32F1::StreamDriver. Too many driver instances." );
    }
}


////////////////////////
void StreamDriver::start( UART_HandleTypeDef* uartHdl ) noexcept
{
    // If in the wrong state -->get in the correct state
    if ( m_uartHdl != nullptr )
    {
        stop();
    }

    // finish the HAL/Driver mapping
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_SMT32F1_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].driver == this )
        {
            m_mappings[idx].halHandle = uartHdl;
            break;
        }
    }

    // Housekeeping
    m_uartHdl = uartHdl;
    m_txBuffer.clearTheBuffer();
    m_rxBuffer.clearTheBuffer();

    // Register ISR callbacks
    HAL_UART_RegisterCallback( m_uartHdl, HAL_UART_TX_COMPLETE_CB_ID, su_txCompleteCallback );
}

void StreamDriver::stop( void ) noexcept
{
    // Stop any in-progress transactions
    HAL_UART_Abort_IT( m_uartHdl );

    // Unregister ISR callback
    HAL_UART_UnRegisterCallback( m_uartHdl, HAL_UART_TX_COMPLETE_CB_ID );

    // Remove HAL/driver mapping
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_SMT32F1_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].driver == this )
        {
            m_mappings[idx].halHandle = nullptr;
            break;
        }
    }

    // Free up any blocked clients (if there are any)
    if ( m_txWaiterPtr )
    {
        m_txWaiterPtr->signal();
    }
    if ( m_rxWaiterPtr )
    {
        m_rxWaiterPtr->signal();
    }

    // Housekeeping
    m_txWaiterPtr = nullptr;
    m_rxWaiterPtr = nullptr;
    m_uartHdl     = nullptr;
}



////////////////////////
bool StreamDriver::write( const void* data, size_t numBytesToTx ) noexcept
{
    // Fail the write if the driver has not been started
    if ( m_uartHdl == nullptr )
    {
        return false;
    }

    // Do nothing if no data needs to be transmitted
    if ( numBytesToTx == 0 )
    {
        return true;
    }

    // Loop until all data has been transferred to the outbound buffer
    uint8_t* ptr = (uint8_t*) data;
    for ( ;;)
    {
        // Transfer my data to the outbound buffer
        while ( numBytesToTx )
        {
            // Populate Ring Buffer
            bool result = m_txBuffer.add( *ptr );

            // Get the next byte if my Ring Buffer was not full
            if ( result )
            {
                ptr++;
                numBytesToTx--;
            }

            // Ring Buffer is full -->skip to transmitting
            else
            {
                break;
            }
        }

        // If not all my outbound data fits in the buffer -->I will block
        if ( numBytesToTx )
        {
            Cpl::System::Thread& myThread = Cpl::System::Thread::getCurrent();

            // INTERRUPT/CRITICAL SECTION: Set Waiter
            Bsp_Api_disableIrqs();
            m_txWaiterPtr = &myThread;
            Bsp_Api_enableIrqs();
        }

        // The HAL expects a 'flat' buffer (not a ring buffer) - so we have to
        // extract the 'flat portion' of ring buffer to pass to the HAL.
        uint8_t* txPtr = m_txBuffer.peekNextRemoveItems( m_txSize );

        // Start transmitting 
        // Note: If there is TX in progress the HAL_UART_Transmit_IT() function does nothing
        if ( txPtr )
        {
            HAL_UART_Transmit_IT( m_uartHdl, txPtr, m_txSize );
        }

        // Wait (if necessary) for buffer to be transmitted/drained
        if ( numBytesToTx )
        {
            Cpl::System::Thread::wait();
        }

        // All done -->exit outer loop
        else
        {
            break;
        }
    }

    return true;
}

int StreamDriver::su_txDoneIsr( void ) noexcept
{
    // Remove the transmitted bytes from TX FIFO
    m_txBuffer.removeElements( m_txSize );

    // Start a new transmit if the TX FIFO is NOT empty
    uint8_t* txPtr = m_txBuffer.peekNextRemoveItems( m_txSize );
    if ( txPtr != nullptr )
    {
        // Start transmitting 
        HAL_UART_Transmit_IT( m_uartHdl, txPtr, m_txSize );
    }

    // By definition if I get here - there is space available in the TX FIFO.
    // Unblock waiting client (if there is one) now that there is space in the FIFO
    int result = 0;
    if ( m_txWaiterPtr )
    {
        result        = m_txWaiterPtr->su_signal();
        m_txWaiterPtr = nullptr;
    }

    return result;
}

void StreamDriver::su_txCompleteCallback( UART_HandleTypeDef* huart ) noexcept
{
    // Look-up the driver instance from the HAL handle
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_SMT32F1_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].halHandle == huart && m_mappings[idx].driver != nullptr )
        {
            m_mappings[idx].driver->su_txDoneIsr();
            break;
        }
    }
}

