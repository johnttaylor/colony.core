#ifndef Cpl_Timer_CounterList_h_
#define Cpl_Timer_CounterList_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

#include "Cpl/Timer/Counter.h"
#include "Cpl/Container/DList.h"


///
namespace Cpl { namespace Timer {

/** This mostly concrete class implements a list of one-shot tick countdown 
    objects (aka active Timers).  When an individual countdown/active object's 
    count reaches zero, its callback method is invoked and it is remove from the
    Active List.  The Active List is designed to be very efficient with
    respect to decrement counts when there are many countdown/active objects
    in the list, i.e. the amount of time to decrement the individual counters
    is NOT a function of the number of active counters.

    The Counter interface is NOT inherently thread safe.  It is designed to be
    use with 'Local Timers'

 */
class CounterList: public CounterSource
{
public:
    /// Constructor
    CounterList();


public: 
    /** This method is intended to be call by a the timing source and each
        call to this method represents that one tick has expired, i.e. decrement
        the active Counter objects' by one.
     */ 
    virtual void tick( unsigned long nticks=1 ) throw();

    /** This method is used by the Tick source to information the Counter list
        that there are no more ticks for the timing source's current tick cycle
     */
    virtual void tickComplete( void ) throw();
     
public:
    /// Returns true if there are NO counters in the list
    bool isEmpty(void) throw();


public: 
    ///  See CounterSource
    void attach( CounterCallback& clientToCallback ) throw();
    
    ///  See CounterSource
    bool detach( CounterCallback& clientToCallback ) throw();


protected:
    /// Helper method.
    void addToActiveList( CounterCallback& clientToCallback ) throw();


protected:
    /// List of active counters
    Cpl::Container::DList<CounterCallback> m_counters;

    /// List of Pending-to-attach counters (this happens when timers attach from the timer-expired-callbacks)
    Cpl::Container::DList<CounterCallback> m_pendingAttach;


    /// Flag to tracks when I am actively processing/consuming ticks
    bool m_inTickCall;
};


};      // end namespaces
};
#endif  // end header latch
