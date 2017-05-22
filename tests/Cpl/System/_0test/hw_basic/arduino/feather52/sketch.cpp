#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Arduino.h"

extern void runtests( void );
extern Cpl::Io::Output& Bsp_Serial( void );

#define SECT_       "_0test"

void setup( void )
{
    // Initialize the board (for use with CPL)
    Bsp_Api_initialize();
    Bsp_beginArduinoSerialObject( 115200, SERIAL_8N1 );

    // Initialize CPL
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eNONE );
    //CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
  //  CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eBRIEF );
}


void loop( void )
{
    // Go run the test(s) 
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Trace: Main Arduino loop") );
    runtests();
    delay( 250 );
}


