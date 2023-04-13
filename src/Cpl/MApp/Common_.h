#ifndef Cpl_MApp_Common_h_
#define Cpl_MApp_Common_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/MApp/Api.h"
#include "Cpl/Container/SList.h"

/*----------------------------------------------------------------------------*/
///
namespace Cpl {
///
namespace MApp {


/** This partial concrete class provide common infrastructure that is common
    to all MApp instances.
 */
class Common_: public Api
{
public:
    /// Constructor.  
    Common_( Cpl::Container::SList<Api>&    mappList, 
             const char*                    mappName,
             const char*                    description,
             const char*                    usage );

public:
    /// See Cpl::MApp::Api
    const char* getName() const noexcept;

    /// See Cpl::MApp::Api
    const char* getDescription() const noexcept;

    /// See Cpl::MApp::Api
    const char* getUsage() const noexcept;

protected:
    /// Command name
    const char* m_name;

    /// Description
    const char* m_description;

    /// Usage
    const char* m_usage;

    /// Started state
    bool        m_started;
};

};      // end namespaces
};
#endif  // end header latch
