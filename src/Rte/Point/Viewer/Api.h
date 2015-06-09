#ifndef Rte_Point_Viewer_Api_h_
#define Rte_Point_Viewer_Api_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */


#include "Rte/Point/Model/Api.h"


///
namespace Rte { namespace Point { namespace Viewer {



/** This abstract class defines the basic operation that a client can
    use with respect to a viewer.
    NOTE: ALL METHODS CAN ONLY BE CALLED FROM THE VIEWER's THREAD
 */
class Api
{
public:
    /** This method is used to register/detach for change notifications
        for this Viewer to its associate Model Point. This method returns 
        returns 1 if the viewer was stopped prior to this call, else
        returns 0.  NOTE: If start is called multiple times in row
        (without a call stop) - the 'extra' start calls are ignored.
        
        If 'forceInitialUpdate' is true, then the viewer is assumed
        to be "out-of-date" and an update of the viewer is triggered
        immediately (i.e. the change notification callback will be 
        called).
          
        The 'useValueForDifference' option determines how a change
        is determined.  If useValueForDifference is set to true than
        only a change in value (as determined by each element in the
        Point) triggers a change notification.  If useValueForDifference
        is false, then all/any updates/writes to the Point will trigger a
        change notification. 

        With respect to the composer - The context's method function
        'm_changedCb' is called when the Viewer Points is different from
        the associated Model Point.  The underlying Viewer Point is
        updated to match the Model Point prior to the callback.   
     */
    virtual unsigned startViewing( bool forceInitialUpdate = true, bool useValueForDifference = true ) = 0;

    /** This method is used to cancel change notifications for this 
        Viewer to its associate Model Point.  NOTE: If stop is called 
        multiple times in row (without a call start) - the 'extra' 
        stop calls are ignored.  The method returns true once the
        viewer has completed stopped (i.e. reclaimed its resources from
        the Model); else false is returned.  This means that the first
        call to stopViewing() will typically return false.
        
        With respec to the composer - When the cancelation process is
        complete, the context's method function 'm_stoppedCb' is called.
     */
    virtual bool stopViewing( void ) = 0;

    /** This method is used to force an change notification from the
        Model Point, i.e. the '_changedCb' will be called with the 
        latest Model data - it works EVEN IF THE VIEWER has not been
        started. The method returns true when succesfull; else false
        is returned.  False is returned when the viewer is in the
        'STOPPING' state and does own the memory to store the poll
        results.
     */
    virtual bool poll( void ) = 0;

    /** Returns a reference to the Model Point the viewer is viewing
     */
    virtual Rte::Point::Model::Api& getModelPoint(void) = 0;

    /** This method returns the Viewer's underling Point object.
        USE THIS METHOD WITH EXTREME CAUTION (i.e. do NOT use it
        unless you really know what you are doing!)
     */
    virtual Rte::Point::Api& getMyPoint(void) = 0;


public:
    /// Make the compiler happy
    virtual ~Api(){}
};


};      // end namespaces
};
};
#endif  // end header latch
