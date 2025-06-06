#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Cpl/TShell/Restartable.h"




// External references
extern Cpl::TShell::Restartable& shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Io::Stdio::StdIn                           infd_;
Cpl::Io::Stdio::StdOut                          outfd_;

// Note: this is global variable for test purposes
Cpl::Container::SList<Cpl::TShell::Command>  cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::Cmd::Threads           threadsCmd_( cmdlist );


int main( int argc, char* const argv[] )
{
	// Initialize Colony
	Cpl::System::Api::initialize();

	CPL_SYSTEM_TRACE_ENABLE();
	CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
	CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

	// Run the test
	Cpl::TShell::Restartable& shell = shell_test( infd_, outfd_ );

	// do not exit
	for ( ;;)
	{
		Cpl::System::Api::sleep( 1000 );
		if ( shell.isRunning() == false )
		{
			shell.restart();
		}
	}

	return 0;
}
