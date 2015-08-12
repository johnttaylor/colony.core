#ifndef Cpl_System_FreeRTOS_Thread_h_
#define Cpl_System_FreeRTOS_Thread_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */ 

#include "Cpl/System/Thread.h"
#include "Cpl/System/Tls.h"
#include "Cpl/Text/FString.h"
#include "task.h"

// Forward declaration for making the Tls class a friend
namespace Cpl { namespace System {
class Tls;
};};


///
namespace Cpl { namespace System { namespace FreeRTOS {

/** This concrete class implements a Thread object using FreeRTOS threads
 */
class Thread: public Cpl::System::Thread 
{
protected:
    /// Reference to the runnable object for the thread
    Cpl::System::Runnable&  m_runnable;         

    /// ASCII name of the task
    Cpl::Text::FString<configMAX_TASK_NAME_LEN>  m_name;
    
    /// internal handle
    TaskHandle_t            m_threadHandle;

    /// Thread Local storage
    void*                   m_tlsArray[OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES];


public: 
    /** Constructor.
            o Does NOT support the application supplying the stack
              memory. Stack memory is allocated from the HEAP

            o If zero is passed as the stack size, then the default stack size
              is set based on the value from the 'FreeRTOSCConfig.h' header file
     */
    Thread( Runnable&      runnable,
            const char*    name,
            int            priority      = CPL_SYSTEM_THREAD_PRIORITY_NORMAL,
            unsigned       stackSize     = 0
          );

    /// Destructor
    ~Thread();

public: 
    /// See Cpl::System::Thread
    const char* getName() throw();

    /// See Cpl::System::Thread
    size_t getId() throw();    

    /// See Cpl::System::Thread
    bool isRunning(void) throw();

    /// See Cpl::System::Thread
    Cpl_System_Thread_NativeHdl_T getNativeHandle(void) throw();


public: 
    /// See Cpl::System::Signable
    int signal(void) throw();

    /// See Cpl::System::Signable
    int su_signal(void) throw();



private:
    /// Entry point for all newly created threads
    static void entryPoint(void* data);

protected:
    /// Returns access to the TLS key array
    static void** getTlsArray() throw();    

public:
    /// Housekeeping
    friend class Cpl::System::Thread;
    friend class Cpl::System::Tls;
};


};      // end namespaces
};
};
#endif  // end header latch
