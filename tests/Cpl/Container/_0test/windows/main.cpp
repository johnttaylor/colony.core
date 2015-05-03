#include "Cpl/System/Api.h"

#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"

// External references
extern void link_slist(void);
extern void link_dlist(void);
extern void link_map(void);
extern void link_dictionary(void);
extern void link_ringbuffer(void);
extern void link_stack(void);


int main( int argc, char* const argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    // THIS CODE DOES NOTHING.  It is needed to force the inclusion of
    // the test code due to the combination of how CATCH auto registers
    // test cases and how NQBP links by libraries.  Short version is do NOT
    // remove these call(s).
    link_slist();
    link_dlist();
    link_map();
    link_dictionary();
    link_ringbuffer();
    link_stack();


    // Run the test(s)
    int result = Catch::Session().run( argc, argv );
    return result;
}
