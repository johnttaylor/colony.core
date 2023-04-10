#ifndef Loki_Test_Test_h_
#define Loki_Test_Test_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Loki/Test/TestApi.h"
#include "Cpl/Container/Map.h"

/*----------------------------------------------------------------------------*/
///
namespace Loki {
///
namespace Test {


/** This partial concrete class provide common infrastructure that is common
    to all "Test" instances.
 */
class Test: public TestApi
{
public:
    /// Constructor.  
    Test( Cpl::Container::Map<TestApi>& testList, const char* testName );

public:
    /// See Loki::Test::TestApi
    const char* getName() const noexcept;

    /// See Loki::Test::TestApi
    bool isPaused() const noexcept;

protected:
    /// See Cpl::Container::Key
    const Cpl::Container::Key& getKey() const noexcept;

protected:
    /// Command 
    Cpl::Container::KeyLiteralString  m_mapKey;

    /// Started state
    bool                              m_started;

    /// Run/Pause state
    bool                              m_running;
};

};      // end namespaces
};
#endif  // end header latch
