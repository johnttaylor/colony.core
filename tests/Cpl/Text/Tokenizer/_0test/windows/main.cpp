#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"


#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"

// External references
extern void link_basic(void);
extern void link_textblock(void);


int main( int argc, char* const argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );

    // THIS CODE DOES NOTHING.  It is needed to force the inclusion of
    // the test code due to the combination of how CATCH auto registers
    // test cases and how NQBP links by libraries.  Short version is do NOT
    // remove these call(s).
    link_basic();
    link_textblock();


    // Run the test(s)
    int result = Catch::Session().run( argc, argv );
    return result;
}
