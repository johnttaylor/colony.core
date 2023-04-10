#ifndef Cpl_MApp_Api_h_
#define Cpl_MApp_Api_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Container/Map.h"
#include "Cpl/MApp/Api.h"
#include "Cpl/Text/String.h"

///
namespace Cpl {
///
namespace MApp {


/** This abstract class defines the "manager" interface to for starting,
    stopping, etc. individual MApp instances.
 */
class Api 
{
public:
    /** This method is used to the specified MApp.  If a MApp is currently executing,
        that MApp will be stopped before the new MApp is started. If the specified 
        MApp name is not found false is returned; else true is returned. 
     */
    virtual bool startMApp( const char* mappName ) noexcept = 0;

    /** This method is used to stop the specified running MApp.  If the MApp
        is currently not in the started state, then the method does nothing
        and false is returned; else true is returned.
     */
    virtual void stopCurrentMApp( const char* mappName ) noexcept = 0;

    /** This method is used to pause the specified running MApp.  If the MApp
        is NOT started (or the MApp is already paused), then the method does nothing.
     */
    virtual void pauseCurrentMApp( const char* mappName ) noexcept = 0;

    /** This method is used to resume the specified running MApp.  If the MApp
        is NOT running (or the MApp is not in the paused state), then the 
        method does nothing.
     */
    virtual void resumeCurrentMApp( const char* mappName ) noexcept = 0;

    /** This method is used to stop ALL currently started MApp instances.
     */
    virtual void stopAllMApps() noexcept = 0;

public:
    /** This method returns a list of available tests.  
     */
    virtual const Cpl::Container::Map<Cpl::MApp::Api>& getAvailableMApps() const noexcept = 0;

    /** This method returns a list of started MApps.  If the number of started
        MApps exceeded the provided buffer space false is returned; else
        return is returned.
     */
    virtual bool getStartedMApps( Cpl::MApp::Api* dstList[], size_t dstMaxElements ) noexcept = 0;

    /** This method returns a pointer to the named MApp instance.  If no such
        MApp exists, a null pointer is returned;
     */
    virtual Cpl::MApp::Api* lookUpMApp( const char* mappName ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~Api() {}
};

};      // end namespaces
};
#endif  // end header latch
