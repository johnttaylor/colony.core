#ifndef Cpl_Itc_EventApi_h_
#define Cpl_Itc_EventApi_h_
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

#include "colony_config.h"
#include <stdint.h>

/// Default the Event Flags data type to 31 Events flags (32-1)
#ifndef Cpl_Itc_EventFlags_T
#define Cpl_Itc_EventFlags_T            uint32_t
#endif

///
namespace Cpl { namespace Itc {



/** This abstract class defines the interface to generated 'Event Flags'. Event
    Flags are used to indicate that an 'event' has occurred.  Each Event Flag
    is represent by a bit the Cpl_Itc_Event_Flags_T data type.  However, one
    Event flag (the MSb) is reserved for internal use.  For example, if
    Cpl_Itc_Event_Flags_T is defined as uint32_t, the application has 31 Event
    Flags (per thread/runnable object) at its disposal. 

    Individual event flags can be viewed as binary semaphores with respect 
    to be signaled/waiting (though waiting is done on the thread's entire set 
    of event flags).

    A thread (runnable object) can wait for at least one event to be signaled.
    When the thread is waiting on event(s) and it is then signaled - all of
    Events that were in the signaled state when the thread was unblock are
    cleared. 
 */

class EventApi
{
public:
    /** This operation is called by clients to set one or more Event Flags.  
        Each bit in 'events' is different Event Flag.  The number of Event
        Flags that can be set are the total number of bits in Cpl_Itc_EventFlags_T
        minus 1.  The MSb of Cpl_Itc_EventFlags_T is reserved and should NEVER
        set by the application.  If the application attempts to set the MSb of 
        'events' then the function will generate a fatal error.  

        This method can ONLY be called from a thread context.
     */
    virtual void notifyEvents( Cpl_Itc_EventFlags_T events ) throw();

    /** This operation is similar to notifyEvents(), except that it sets one 
        event flags.  The 'eventNumber' is the bit number (zero based) of the 
        Event Flag to set.  This method will generate a fatal error if the MSb 
        bit number is specified (see limitations of notifyEvents()

        This method can ONLY be called from a thread context.
     */
    virtual void notify( uint8_t eventNumber  ) throw();


public:
    /** This method is same as notifyEvents() EXCEPT this method can ONLY be 
        called from supervisor mode and/or ISR contexts. 
     */
    virtual void su_notifyEvents( Cpl_Itc_EventFlags_T events ) throw();

    /** This method is same as notify() EXCEPT this method can ONLY be 
        called from supervisor mode and/or ISR contexts. 
     */
    virtual void su_notify( uint8_t eventNumber ) throw();


public:
    /** This method is PRIVATE, it is only made public to avoid the tight
        coupling of the 'friend mechanism' for friending current and future
        concrete thread classes.  The application SHOULD NEVER call/use this
        method.

        This internal method can set any/all Event Flags, including the 
        MSb event flag.

        This method can ONLY be called from a thread context.
     */
    virtual void internalNotify_( Cpl_Itc_EventFlags_T events ) throw()=0;

    /** This method is PRIVATE, it is only made public to avoid the tight
        coupling of the 'friend mechanism' for friending current and future
        concrete thread classes.  The application SHOULD NEVER call/use this
        method.
        
        This method is same as internalNotify_() EXCEPT this method can ONLY be 
        called from supervisor mode and/or ISR contexts. 
     */
    virtual void su_internalNotify_( Cpl_Itc_EventFlags_T events ) throw()=0;


public:
	/// Virtual destructor
	virtual ~EventApi(){}
};

};      // end namespaces
};
#endif  // end header latch

