#include "Cpl/System/Api.h"
#include "Cpl/System/Shutdown.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Memory/_testsupport/New_TS.h"


#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"

// External references
extern void link_fstring(void);
extern void link_dstring(void);
extern void link_dfstring(void);
extern void link_string(void);
extern void link_strip(void);
extern void link_atob(void);
extern void link_btoa(void);
extern void link_format(void);
extern void link_stringItem(void);


int main( int argc, char* const argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );
    CPL_SYSTEM_TRACE_FUNC( "_0test");

    // THIS CODE DOES NOTHING.  It is needed to force the inclusion of
    // the test code due to the combination of how CATCH auto registers
    // test cases and how NQBP links by libraries.  Short version is do NOT
    // remove these call(s).
    link_fstring();
    link_dstring();
    link_dfstring();
    link_string();
    link_strip();
    link_atob();
    link_btoa();
    link_format();
    link_stringItem();

	// Expect EQUAL new/delete calls
	// Ignoring memory stats on Windoze -->can't account for 'extra' news
    Cpl::Memory::New_TS::setNewDelete_delta( 0, true );


    // Run the test(s)
    int result = Catch::Session().run( argc, argv );

	// Shutdown Cpl so my testing infra-structure cleanup/post processing gets done
    Cpl::System::Shutdown_TS::restore();
    return Cpl::System::Shutdown::failure( result );
}
