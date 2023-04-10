#ifndef Cpl_MApp_Api_h_
#define Cpl_MApp_Api_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "colony_config.h"
#include "Cpl/Container/MapItem.h"


/// CPL Trace Section identifier for a common trace output section
#ifndef OPTION_CPL_MAPP_TRACE_SECTION
#define OPTION_CPL_MAPP_TRACE_SECTION   "MApp"
#endif

///
namespace Cpl {
///
namespace MApp {


/** This abstract class defines the interface for a Micro Application (MApp).  
    A MApp is essentially a 'mini-application' that performs a series of steps 
    and typically reports its output using the CPL_SYSTEM_TRACE_MSG() method.

    Multiple MApp instances can be executing at the same.  However, all MApps 
    execute in a single/same thread.

    The following are the 'thread safety' requirements for a MApp:
        o Data can safely be exchanged via Model Points
        o If a MApp is in the stopped or paused state - it can exchange information
          with the Clients without using critical sections.  It is the responsibility
          of the individual MApp instances to enforce this convention.
 */
class Api : public Cpl::Container::MapItem
{
public:
    /** This method returns the MApp name.  The MApp name must an printable
        ASCII string with NO whitespace.  The name is used to uniquely
        identify the individual MApp
     */
    virtual const char* getName() const noexcept = 0;

    /** This method returns help/instruction for the MApp.  Typically this
        information will be displayed on the Debug console to aid the user on
        how to execute the MApp.  The content/verbosity of the help text is
        MApp specific.
     */
    virtual const char* getHelp() const noexcept = 0;


public:
    /** This method is used to perform any needed initialization on start-up of
        Application. This method is called when the MApp Manager is opened.
     */
    virtual void intialize() noexcept = 0;

    /** This method is used to perform any needed shutdown when the application
        is shutdown.  This method is called when the MApp Manager is closed.
     */
    virtual void shutdown() noexcept = 0;


public:
    /// This method is used to start a MApp.  
    virtual void start() noexcept = 0;

    /// This method is used to stop a MApp.
    virtual void stop() noexcept = 0;

    /** This method is used to pause a MApp.  What 'pause' means is MApp
        specific.
     */
    virtual void pause() noexcept = 0;

    /** This method is used to resume a pause a MApp.
     */
    virtual void resume() noexcept = 0;

    /// This method returns trues if the MApp is in the started state
    virtual bool isStarted() noexcept = 0;

    /// This method returns true if the MApp is in the started state AND is it has been paused
    virtual bool isPaused() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~Api() {}
};

};      // end namespaces
};
#endif  // end header latch
