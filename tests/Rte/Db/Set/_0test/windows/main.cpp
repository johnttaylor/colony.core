#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"

#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"

// External references
extern void link_set(void);


int main( int argc, char* const argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    // THIS CODE DOES NOTHING.  It is needed to force the inclusion of
    // the test code due to the combination of how CATCH auto registers
    // test cases and how NQBP links by libraries.  Short version is do NOT
    // remove these call(s).
    link_set();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_ENABLE_SECTION("Rte::Db::Chunk");
    CPL_SYSTEM_TRACE_ENABLE_SECTION("Rte::Db::Record");
    CPL_SYSTEM_TRACE_ENABLE_SECTION("Rte::Db::Record::Fsm");
    CPL_SYSTEM_TRACE_ENABLE_SECTION("Rte::Db::Set");
    CPL_SYSTEM_TRACE_ENABLE_SECTION("Rte::Db::Set::Fsm");
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eBRIEF );

    // Run the test(s)
    int result = Catch::Session().run( argc, argv );
    return result;
}
