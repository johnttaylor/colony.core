#include "Cpl/System/Api.h"
#include "Cpl/System/Shutdown.h"
#include "Cpl/Memory/_testsupport/New_TS.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"


#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"

int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

	// With the gcc 5.3.1 compiler the new/delete calls no longer match (hmm...)
    Cpl::Memory::New_TS::setNewDelete_delta( 0, true );


    // Run the test(s)
    int result = Catch::Session().run( argc, argv );

	// Shutdown Cpl so my testing infra-structure cleanup/post processing gets done
    Cpl::System::Shutdown_TS::restore();
    return Cpl::System::Shutdown::failure( result );
}

