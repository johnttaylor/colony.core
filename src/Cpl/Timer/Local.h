#ifndef Cpl_Timer_Local_h_
#define Cpl_Timer_Local_h_
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

#include "Cpl/Timer/Api.h"
#include "Cpl/Timer/Counter.h"
#include "Cpl/System/Trace.h"


///
namespace Cpl { namespace Timer {

/** This template class implements a Local Timer that is context
    independent and allows for a single context to contain many Timers.

    A Local timer is defined as timer that has its tick source in the
    SAME thread.  Because the timer methods and callback all occur in the
    the same thread, the CONTEXT will never receive a timer expired callback 
    AFTER the timer's stop() method has been called.

    NOTES: 

        o Because the timing source of an individual thread may NOT be a 
          clean divider of the timer duration, the timer duration is taken
          as the minimum.  For example: if the timing source has a resolution
          of 20msec per count, and the timer duration on the start() timer
          call is 5 msec, then the timer will expire after the next full count,
          i.e. after 20msec, not 5msec.  IT IS THE APPLICATION'S RESPONSIBILITY
          TO MANAGE THE RESOLUTION OF THE TIMING SOURCES.

    Template args:
        CONTEXT   Type of the Class that implements the context for the timer
  */

template <class CONTEXT>
class Local: public Api,
             public CounterCallback
{
public:         
    /** Definition of the call-back method that notifies the 
        context/client when the timer expires.
     */
    typedef void (CONTEXT::* TimerExpiredFunction_T)();

private:
    ///
    CounterSource&          m_countSource;
    ///
    unsigned long           m_count;
    ///
    CONTEXT&                m_context;
    ///
    TimerExpiredFunction_T  m_expiredFuncPtr;


public:
    /// Constructor
    Local( CounterSource&         localCountSource,
           CONTEXT&               timerContextInstance,
           TimerExpiredFunction_T expiredCallbackFunc
         );

public: 
    /// See Cpl::Timer::Api
    void start( unsigned long timerDurationInMilliseconds ) throw();


    /// See Cpl::Timer::Api
    void stop() throw();


private: // TickLocalCounterObserverApi
    ///
    void expired(void) throw();
    ///
    void decrement( unsigned long nticks=1 ) throw();
    ///
    void increment( unsigned long nticks ) throw();
    ///
    void set( unsigned long nticks ) throw();
    ///
    unsigned long count() const throw();
};


//==========================================================================
//                          IMPLEMENTATION
//==========================================================================

/////////////////////
template <class CONTEXT>
Local<CONTEXT>::Local
    ( 
    CounterSource&          localTickSource,
    CONTEXT&                context,
    TimerExpiredFunction_T  expiredCallback
    )
:m_countSource(localTickSource),
 m_count(0),
 m_context(context),
 m_expiredFuncPtr(expiredCallback)
    {
    }


/////////////////////
template <class CONTEXT>
void Local<CONTEXT>::start( unsigned long timerDurationInMilliseconds ) throw()
    {
    m_countSource.detach(*this);
    m_count = m_countSource.msecToCounts( timerDurationInMilliseconds );
    m_countSource.attach(*this);
    }

template <class CONTEXT>
void Local<CONTEXT>::stop() throw()
    {
    m_countSource.detach(*this);
    }


/////////////////////
template <class CONTEXT>
void Local<CONTEXT>::decrement( unsigned long nticks ) throw()
    {
    m_count -= nticks;
    }

template <class CONTEXT>
void Local<CONTEXT>::increment( unsigned long nticks ) throw()
    {
    m_count += nticks;
    }

template <class CONTEXT>
unsigned long Local<CONTEXT>::count( void ) const throw()
    {
    return m_count;
    }

template <class CONTEXT>
void Local<CONTEXT>::set( unsigned long nticks ) throw()
    {
    m_count = nticks;
    }

template <class CONTEXT>
void Local<CONTEXT>::expired( void ) throw()
    {
    (m_context.*m_expiredFuncPtr)();
    }

};      // end namespaces
};
#endif  // end header latch
