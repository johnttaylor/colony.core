#ifndef Cpl_Timer_Api_h_
#define Cpl_Timer_Api_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */


///
namespace Cpl { namespace Timer {


/** This abstract interface defines the operations that can be performed
    on a timer (from the application usage perspective).  The details of
    the timer - in-thread vs. inter-thread, etc. behavior are determined by 
    the concrete child classes.
 */
class Api
{
public:
    /** Starts the timer with an initial count down count duration of 
        'timerDurationInMilliseconds'
     */
    virtual void start( unsigned long timerDurationInMilliseconds ) throw() = 0;

    /** Effectively performs a stop() followed by a start() call 
     */
    virtual void restart( unsigned long timerDurationInMilliseconds ) throw() = 0;

    /** Stops the timer. It is okay to call stop() even after the timer has
        previously expired or explicity stopped.  Note: The behavior for the use 
        case of: timer-expired-notification-in-progress when stop() is called is 
        dependent on the actual timer implementaiton (in-thread vs. inter-thread).
     */
    virtual void stop() throw() = 0;


public:
    /// Virtual destructor
    virtual ~Api(){}
};


};      // end namespaces
};
#endif  // end header latch
