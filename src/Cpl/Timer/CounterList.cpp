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

#include "CounterList.h"
#include "Cpl/System/Trace.h"


///
using namespace Cpl::Timer;

#define SECT_   "Cpl::Timer"


///////////////////////////
CounterList::CounterList()
:m_inTickCall(false)
    {
    }


bool CounterList::isEmpty(void) throw()
    {
    return m_counters.first() == 0 ;
    }



/////////////////////////
void CounterList::tick( unsigned long nticks ) throw()
    {
    // Set my state to: PROCESSING TICK(S)    
    m_inTickCall = true;

    while( nticks )
        {
        CounterCallback* counterPtr = m_counters.first();
        if ( counterPtr == 0 )
            {
            // No timers registered -->do NOTHING
            break;
            }
        else
            {
            // Decrement the first/active counter in the list
            unsigned long nextCount = counterPtr->count();
            if ( nticks <= nextCount )
                {
                counterPtr->decrement( nticks );
                nticks = 0;
                }
            else
                {
                counterPtr->decrement( nextCount );
                nticks -= nextCount; 
                }

            // Process ALL local Timers that have a ZERO countdown value
            while( counterPtr && counterPtr->count() == 0 )
                {
                m_counters.get();                // Remove the expired counter from the list
                counterPtr->expired();           // Expire the counter
                counterPtr = m_counters.first(); // Get next counter
                }
            }
        }
    }

void CounterList::tickComplete( void ) throw()
    {
    // Process pending attaches now that the tick cycle has completed
    CounterCallback* pendingClientPtr = m_pendingAttach.get();
    while( pendingClientPtr )
        {
        addToActiveList( *pendingClientPtr );
        pendingClientPtr = m_pendingAttach.get();
        }

    // Clear my PROCESSING TICK(S) state
    m_inTickCall = false;
    }


/////////////////////////
void CounterList::attach( CounterCallback& clientToCallback ) throw()
    {
    // Trap the case of the counter is ALREADY zero
    if ( clientToCallback.count() == 0 )
        {
        clientToCallback.expired();
        return;
        }

    // Do NOT add to my active timer list while I am processing tick(s)!
    if ( m_inTickCall )
        {
        m_pendingAttach.put( clientToCallback );
        }

    // Add client timer
    else
        {
        addToActiveList( clientToCallback );
        }
    }


void CounterList::addToActiveList( CounterCallback& clientToCallback ) throw()
    {
    // Insert the counter wisely into the list.  The counters are
    // stored in the list in the order they will expire and have their
    // raw counter value adjusted to be relative to any/all preceeding list
    // elements.  This allows me to only decrement the first counter in 
    // the list - instead of all counters every tick.  
    CounterCallback* counterPtr = m_counters.first();
    while( counterPtr )
        {
        // Insert at the head of the list
        if ( clientToCallback.count() < counterPtr->count() )
            {          
            counterPtr->decrement( clientToCallback.count() );
            m_counters.insertBefore( *counterPtr, clientToCallback );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( ">> INSERT: %p, count=%lu, : BEFORE %p (%lu)", &clientToCallback, clientToCallback.count(), counterPtr, counterPtr->count() ));
            return;
            }

        // Insert in the middle
        clientToCallback.decrement( counterPtr->count() );
        if ( clientToCallback.count() == 0 )
            {
            m_counters.insertAfter( *counterPtr, clientToCallback );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( ">> INSERT:: %p, count=%lu, AFTER %p (%lu)", &clientToCallback, clientToCallback.count(), counterPtr, counterPtr->count() ));
            return;
            }            

        counterPtr = m_counters.next( *counterPtr );
        }

    // Insert at the tail (list is empty or largest counter value)
    CPL_SYSTEM_TRACE_MSG( SECT_, ( ">> INSERT @ end: %p, count=%lu", &clientToCallback, clientToCallback.count() ));
    m_counters.putLast( clientToCallback );
    }


bool CounterList::detach( CounterCallback& clientToCallback ) throw()
    {
    // Try my pending list FIRST
    if ( m_pendingAttach.remove( clientToCallback ) )
        {
        return true;
        }

    // If I have the counter/timer -->it will be in the active list.  
    CounterCallback* counterPtr = m_counters.first();
    while( counterPtr )
        {
        if ( counterPtr == &clientToCallback )
            {
            // Add the remaining time of the counter being remove to the next counter in the list
            CounterCallback* nextPtr = m_counters.next( *counterPtr );
            if ( nextPtr )
                {
                nextPtr->increment( counterPtr->count() );
                }

            // remove the counter
            m_counters.remove( *counterPtr );
            return true;
            }

        counterPtr = m_counters.next( *counterPtr );
        }

    // If I get here, the Counter was NOT in the active list (AND it was not in the staging list)
    return false;
    }



