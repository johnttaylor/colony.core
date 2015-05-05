#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"


//#define CATCH_CONFIG_RUNNER  
//#include "Catch/catch.hpp"

// External references
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Io::Stdio::StdIn   infd_;
Cpl::Io::Stdio::StdOut  outfd_;

int main( int argc, char* const argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

    // THIS CODE DOES NOTHING.  It is needed to force the inclusion of
    // the test code due to the combination of how CATCH auto registers
    // test cases and how NQBP links by libraries.  Short version is do NOT
    // remove these call(s).
    //link_basic();
    //link_textblock();


    // Run the test(s)
    //int result = Catch::Session().run( argc, argv );
    shell_test( infd_, outfd_ );

    // do not exit
    for(;;)
        {
        Cpl::System::Api::sleep(1000);
        }

    //return result;
	return 0;
}
