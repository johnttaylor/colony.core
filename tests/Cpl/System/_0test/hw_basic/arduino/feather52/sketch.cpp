#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"

extern void runtests( void );

void setup( void )
{
    // Initialize the board
    Bsp_Api_initialize();
    Bsp_beginArduinoSerialObject( 115200, SERIAL_8N1 );

    // Initialize CPL
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
//    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eBRIEF );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
}

void loop( void )
{
    CPL_SYSTEM_TRACEMSG( "_0test", ("Hello World\r\n") );

    // Go run the test(s) (Note: This method should never return)
    //runtests();
}



