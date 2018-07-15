#ifndef Cpl_Itc_Mailbox_h_
#define Cpl_Itc_Mailbox_h_
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
#include "Cpl/Itc/PostApi.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/System/FastLock.h"
#include "Cpl/Container/SList.h"


/** Specifies the default timeout period for waiting on message.
 */
#ifndef OPTION_CPL_ITC_MAILBOX_TIMEOUT_PERIOD
#define OPTION_CPL_ITC_MAILBOX_TIMEOUT_PERIOD       1  /// 1 msec timeout, aka 1 msec timer resolution for Local Timers
#endif

           
///
namespace Cpl { namespace Itc { 

/** This mailbox class implements an Inter Thread Communications
    mechanism based on message passing. There is no limit to the
    number of messages that can be stored in the queue at any given
    time since the FIFO queue and the messages uses the intrusive
    container mechanisms from the Cpl::Container namespace.  The
    implementation provides support for implementing (by a child
    class) a time source for Local Timers (Cpl::Timer::Local)
 */

class Mailbox: public PostApi,
               public Cpl::System::Signable,
               public Cpl::Container::SList<Message>
{
public:
    /** Constructor. The 'timeOutPeriodInMsec' parameter specifies how
        long the mailbox will wait for message before returning from
        the waitNext() call.  If 'timeOutPeriodInMsec' is zero, then
        the mailbox will wait forever, i.e. never timeout.  The value
        of 'timeOutPeriodInMsec' is resolution of the timing source
        for Local Timers used by services that are associated with the
        mailbox.
     */
    Mailbox( unsigned long timeOutPeriodInMsec = OPTION_CPL_ITC_MAILBOX_TIMEOUT_PERIOD );


public: 
    /** This operation is used by the owner thread of the mailbox
        to wait for messages to be posted by a client to the mailbox.
        The calling thread blocks until a message is posted,
        the mailbox is signaled, or the specified timeout period
        has expired. When this operation completes, either a pointer to 
        a message, or zero is returned. A zero pointer indicates that 
        the mailbox was signaled or a timeout occurred before a message 
        was posted to the mailbox. The caller can use the 'wasTimeout'
        argument to differentiate between the mailbox being signaled
        and a timeout occurred.
     */
    virtual Message* waitNext( bool& wasTimeout ) throw();


public:
    /// See Cpl::Itc::PostApi
    void post(Message& msg) throw();

    /// See Cpl::Itc::PostApi
    void postSync(Message& msg) throw();


public: 
    /// See Cpl::System::Signable
    int signal(void) throw();

    /// See Cpl::System::Signable
    int su_signal(void) throw();


protected:
    /// Mutex used to protect my list
    Cpl::System::FastLock   m_flock;

    /// Semaphore associated with the mailbox (note: the Thread semaphore is used for managing synchronous ITC calls)
    Cpl::System::Semaphore  m_sema;

    /// Timeout period for waiting on the next message
    unsigned long           m_timeout;

    /// Keep track of when the mailbox is waiting for the next message
    bool                    m_waiting;

    /** The value of this variable is set to true by the Signaled
        interface, when the mailbox is signaled and there is no
        waiter blocked on the mailbox. This results in the implementation
        of a binary semaphore mechanism, which can be used by timers and
        non-message input to the mailbox. Such a signal causes the
        waiter to return from the waitNext function with a null
        message pointer.
     */
    bool                    m_signaled;

};


};      // end namespaces
};
#endif  // end header latch