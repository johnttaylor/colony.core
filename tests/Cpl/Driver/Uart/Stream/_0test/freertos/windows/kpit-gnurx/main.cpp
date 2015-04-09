#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Driver/Uart/Stream/Transmitter.h"
#include "Cpl/Driver/Uart/Stream/Receiver.h"
#include <stdlib.h>


extern void loopback_test( Cpl_Driver_Uart_Hal_T uartHdl, bool manualFirstTx );

extern Cpl::Driver::Uart::Stream::Transmitter* txPtr;
extern Cpl::Driver::Uart::Stream::Receiver*    rxPtr;

               
int main(void)
    {
    // Initialize the board
    Bsp_Api_initialize();
    Bsp_uartInit( BSP_DB9_CONSOLE, BSP_DB9_CONSOLE_PIN_SELECT, configMAX_SYSCALL_INTERRUPT_PRIORITY-2, BSP_BAUDRATE_57600, BSP_BAUDRATE_DIVIDER_57600, (BSP_PARITY_NONE|BSP_DATABITS_8|BSP_STOPBITS_1) );

    // Initialize CPL
    Cpl::System::Api::initialize();

    // Go run the test(s) (Note: This method should never return)
    loopback_test( BSP_DB9_CONSOLE, true );

    // I should never get here!
	for( ;; );
    return 0;
    }



/*-----------------------------------------------------------*/
void consoleTxISR( void )
	{
    static BaseType_t xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = txPtr->su_txDoneIsr_();

    /* If xHigherPriorityTaskWoken was set to true you
    we should yield.  The actual macro used here is 
    port specific. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}


void consoleRxISR( void )
	{
    static BaseType_t xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = rxPtr->su_rxDataAndErrorIsr_();

    /* If xHigherPriorityTaskWoken was set to true you
    we should yield.  The actual macro used here is 
    port specific. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
