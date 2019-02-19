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

#include "Thread.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Private_.h"

//
using namespace Cpl::System::BareMetal;

//
static Thread* mainThread_ = 0;

#define MAIN_THREAD_NAME    "MAIN"

////////////////////////////////////
namespace {

/// This class is used to turn the entry/native/main thread into a Cpl::System::Thread (i.e. add the thread semaphore)    
class RegisterInitHandler_ : public Cpl::System::StartupHook_,
    public Cpl::System::Runnable
{
protected:
    // Empty run function
    // Note: Leave my 'running state' set to false -->this is so I don't 
    // terminate the native thread prematurely when/if the Thread instance
    // is deleted.  In theory this can't happen since the Thread and Runnable
    // instance pointers for the native thread are never exposed to the 
    // application and/or explicitly deleted.
    void appRun() {}

public:
    ///
    RegisterInitHandler_():StartupHook_( eSYSTEM ) {}


protected:
    ///
    void notify( InitLevel_T init_level )
    {
        // Create a thread object for the native thread
        mainThread_ = new Thread( *this );
    }

};
}; // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


////////////////////////////////////
Thread::Thread( Cpl::System::Runnable& dummyRunnable )
    :m_runnable( dummyRunnable )
{
}

Thread::~Thread()
{
    // Nothing needed
}


//////////////////////////////
int Thread::signal() throw()
{
    return m_syncSema.signal();
}

int Thread::su_signal() throw()
{
    return m_syncSema.su_signal();
}

const char* Thread::getName() throw()
{    
    return MAIN_THREAD_NAME;
}

size_t Thread::getId() throw()
{
    return 0;
}

bool Thread::isRunning() throw()
{
    return true;
}

Cpl_System_Thread_NativeHdl_T Thread::getNativeHandle( void ) throw()
{
    return 0;
}


//////////////////////////////


//////////////////////////////
Cpl::System::Thread& Cpl::System::Thread::getCurrent() throw()
{
    // Trap potential error
    if ( !mainThread_ )
    {
        Cpl::System::FatalError::logRaw( "BareMeal::Thread::getCurrent().  Cpl::System::Api::initialize() has NOT been called" );
    }

    return *mainThread_;
}


void Cpl::System::Thread::wait() throw()
{
    mainThread_->m_syncSema.wait();
}

bool Cpl::System::Thread::timedWait( unsigned long timeout ) throw()
{
    return mainThread_->m_syncSema.timedWait( timeout );
}

const char* Cpl::System::Thread::myName() throw()
{
    return MAIN_THREAD_NAME;
}


size_t Cpl::System::Thread::myId() throw()
{
    return 0;
}



//////////////////////////////
void Cpl::System::Thread::traverse( Cpl::System::Thread::Traverser& client ) throw()
{
    client.item( *mainThread_ );
}


//////////////////////////////
Cpl::System::Thread* Cpl::System::Thread::create( Runnable&   runnable,
                                                  const char* name,
                                                  int         priority,
                                                  int         stackSize,
                                                  void*       stackPtr,
                                                  bool        allowSimTicks
)
{
    return 0;
}


void Cpl::System::Thread::destroy( Thread& threadToDestroy )
{
    delete &threadToDestroy;
}
