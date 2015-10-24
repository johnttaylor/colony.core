#include "Cpl/System/Api.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Cpl/TShell/Dac/Cmd/Win32/Threads.h"




// External references
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Io::Stdio::StdIn                           infd_;
Cpl::Io::Stdio::StdOut                          outfd_;

Cpl::Container::Map<Cpl::TShell::Dac::Command>  cmdlist("ignore_this_parameter-used to invoke the static constructor" );

// HACK: to reuse a common test file
#ifndef BUILD_VARIANT_CPP11
Cpl::TShell::Dac::Cmd::Win32::Threads           threadsCmd_( cmdlist );
#else
Cpl::TShell::Dac::Cmd::Threads                  threadsCmd_( cmdlist );
#endif


int main( int argc, char* const argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

	// Run the test
    shell_test( infd_, outfd_ );

    // do not exit
    for(;;)
        {
        Cpl::System::Api::sleep(1000);
        }

	return 0;
}