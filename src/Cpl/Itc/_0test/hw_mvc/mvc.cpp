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

#include "Cpl/Itc/_0test/common.h"
#include "Bsp/Api.h"

#define REQUIRE(a)  if ( !(a) ) Cpl::System::FatalError::logf("Failed Assert")


/// 
using namespace Cpl::Itc;

////////////////////////////////////////////////////////////////////////////////
namespace {

class Checker: public Cpl::System::Runnable
{
public:
    ///
    MyMailboxServer         m_modelMbox;
    ///
    Model                   m_myModel;
    ///
    ViewRequest::SAP        m_modelViewSAP;
    ///
    MailboxServer           m_viewerMbox;
    ///
    Viewer                  m_myViewer;
    ///
    Cpl::System::Thread*    m_viewerThreadPtr;
    ///
    Cpl::System::Thread*    m_modelThreadPtr;

public:
    ///
    Checker()
    :m_modelMbox(),
     m_myModel(m_modelMbox),
     m_modelViewSAP(m_myModel,m_modelMbox),
     m_viewerMbox(),
     m_myViewer( 0, m_viewerMbox, m_modelViewSAP ),
     m_viewerThreadPtr(0),
     m_modelThreadPtr(0)
        {
        }

public:
    ///
    void appRun()
        {
        CPL_SYSTEM_TRACE_FUNC( SECT_ );
        Bsp_Api_turnOn_debug1();    
        Cpl::System::Thread* masterThreadPtr = Cpl::System::Thread::create( *(new Master( NUM_SEQ_, NUM_WRITES_, m_myViewer, m_myModel, m_myModel, Cpl::System::Thread::getCurrent()) ),  
                                                                            "MASTER" );
        
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Starting the sequence...." ));
        int i;
        for(i=0; i< NUM_SEQ_; i++ )
            {
            Bsp_Api_toggle_debug2();    
            m_modelMbox.signal();
            Cpl::System::Thread::wait();
            Cpl::System::Api::sleep(50);
            REQUIRE( m_myModel.m_value == (NUM_WRITES_ - 1) * ATOMIC_MODIFY_ );
            REQUIRE( m_myViewer.m_attachRspMsg.getPayload().m_value == (NUM_WRITES_ - 1)*ATOMIC_MODIFY_ );
            REQUIRE( m_myViewer.m_ownAttachMsg == true );
            REQUIRE( m_myViewer.m_ownDetachMsg == true );
            REQUIRE( m_myViewer.m_pendingCloseMsgPtr == 0 );
            REQUIRE( m_myViewer.m_opened == false );
            REQUIRE( m_modelMbox.m_sigCount == i+1 );
            masterThreadPtr->signal();
            }

        // Wait for all of the sequences to complete
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Sequence Complete: Waiting for all threads to finish" ));
        Bsp_Api_turnOff_debug2();    
        Cpl::System::Thread::wait();

        // Shutdown threads
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Shutting down threads..." ));
        Bsp_Api_turnOff_debug1();    
        m_viewerMbox.pleaseStop();
        m_modelMbox.pleaseStop();
        Cpl::System::Api::sleep(50); // allow time for threads to stop
        REQUIRE( m_viewerThreadPtr->isRunning() == false );
        REQUIRE( m_modelThreadPtr->isRunning() == false );
        REQUIRE( masterThreadPtr->isRunning() == false );

        Cpl::System::Thread::destroy( *m_viewerThreadPtr );
        Cpl::System::Thread::destroy( *m_modelThreadPtr );
        Cpl::System::Thread::destroy( *masterThreadPtr );

        // Test done -->flash LED to indicate succesful test (i.e. not stuck in the FatalError handler)
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "All threads to destroyed!" ));
        for(;;)
            {
            Bsp_Api_toggle_debug1();    
            Cpl::System::Api::sleep(250);
            }
        }
};

};  // end namespace


////////////////////////////////////////////////////////////////////////////////

static Checker checker_;


////////////////////////////////////////////////////////////////////////////////
void runtests( void )
    {
    Cpl::System::Thread::create( checker_, "CHECKER" );
    checker_.m_viewerThreadPtr = Cpl::System::Thread::create( checker_.m_viewerMbox, "Viewer" );
    checker_.m_modelThreadPtr  = Cpl::System::Thread::create( checker_.m_modelMbox,  "Model" );
 
    // Start the schedular
    Cpl::System::Api::enableScheduling();
    }
