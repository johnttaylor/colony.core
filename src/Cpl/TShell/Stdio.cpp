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

#include "Stdio.h"
#include "Cpl/System/Thread.h"


///
using namespace Cpl::TShell;


/// Anonymous namespace
namespace {

class Shell_: public Cpl::System::Runnable
{
public:
    ///
    Processor&       m_shell;
    ///
    Cpl::Io::Input&  m_infd;
    ///
    Cpl::Io::Output& m_outfd;

public:
    Shell_( Processor& shell, Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
        :m_shell(shell)
        ,m_infd(infd)
        ,m_outfd(outfd)
            {
            }

public:
    ///
    void pleaseStop() 
        {
        m_shell.requestStop();
        }

    ///
    void appRun()
        {
        m_shell.start( m_infd, m_outfd );
        }
};
}; // end anonymous namespace


/////////////////////////////////////////
Stdio::Stdio( Processor& shell, const char* threadName, int threadPriority ) throw()
:m_shell(shell)
,m_threadPtr(0)
,m_priority(threadPriority)
,m_name(threadName)
,m_runnablePtr(0)
    {
    }

Stdio::~Stdio()
    {
    delete m_runnablePtr;
    if ( m_threadPtr )
        {
        Cpl::System::Thread::destroy( *m_threadPtr );
        }
    }


/////////////////////////////////////////
void Stdio::launch( Cpl::Io::Input& infd, Cpl::Io::Output& outfd ) throw()
    {
    // If I am re-launched -->kill the previous shell
    delete m_runnablePtr;
    if ( m_threadPtr )
        {
        Cpl::System::Thread::destroy( *m_threadPtr );
        }

    m_runnablePtr = new Shell_( m_shell, infd, outfd );
    m_threadPtr   = Cpl::System::Thread::create( *m_runnablePtr, m_name, m_priority );
    }

