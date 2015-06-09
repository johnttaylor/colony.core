#ifndef Common_h_
#define Common_h_
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

#include "Cpl/Itc/MailboxServer.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"

#include "Cpl/Itc/CloseSync.h"

#include "Rte/Point/Model/_0test/Point/bar1_m.h"   // Note: violating #include rule about not including from sub-directories -->since this is test code I rationalize the cheating!
#include "Rte/Point/Model/_0test/Point/bar1_c.h"
#include "Rte/Point/Model/_0test/Point/bar1_q.h"
#include "Rte/Point/Model/_0test/Point/bar1_v.h"

#include "Rte/Point/Model/_0test/Point/bar2_m.h"
#include "Rte/Point/Model/_0test/Point/bar2_c.h"
#include "Rte/Point/Model/_0test/Point/bar2_q.h"
#include "Rte/Point/Model/_0test/Point/bar2_v.h"

#include "Rte/Point/Model/_0test/Point/bar3_m.h"
#include "Rte/Point/Model/_0test/Point/bar3_c.h"
#include "Rte/Point/Model/_0test/Point/bar3_q.h"
#include "Rte/Point/Model/_0test/Point/bar3_v.h"


#define SECT_           "_0test"



///////////////////////////////////
static const char* b2t_( bool flag )
    {
    return flag? "TRUE": "false";
    }

static void traceFoo1_( Tuple::Foo1& tuple, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    # %s::Foo1: %s", name, msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _name:=[%s]. valid=%s, inUse=%d",  tuple.m_name.get(), b2t_(tuple.m_name.isValid()), tuple.m_name.isInUse()  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _enabled:=%d. valid=%s, inUse=%d", tuple.m_enabled.get(), b2t_(tuple.m_enabled.isValid()), tuple.m_enabled.isInUse()  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _count:=%d. valid=%s, inUse=%d",   tuple.m_count.get(), b2t_(tuple.m_count.isValid()), tuple.m_count.isInUse() ));
    }

static void traceFoo2_( Tuple::Foo2& tuple, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    # %s::Foo2: %s", name, msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _enabled:=%d. valid=%s, inUse=%d", tuple.m_enabled.get(), b2t_(tuple.m_enabled.isValid()), tuple.m_enabled.isInUse()  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _limit:=%d. valid=%s, inUse=%d",   tuple.m_limit.get(), b2t_(tuple.m_limit.isValid()), tuple.m_limit.isInUse() ));
    }

static void traceFoo3_( Tuple::Foo3& tuple, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    # %s::Foo3: %s", name, msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _inContainer_:=%s. valid=%s, inUse=%d",    b2t_(tuple.m_inContainer_.get()), b2t_(tuple.m_inContainer_.isValid()), tuple.m_inContainer_.isInUse()  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _name:=[%s]. valid=%s, inUse=%d",          tuple.m_name.get(), b2t_(tuple.m_name.isValid()), tuple.m_name.isInUse()  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _enabled:=%d. valid=%s, inUse=%d",         tuple.m_enabled.get(), b2t_(tuple.m_enabled.isValid()), tuple.m_enabled.isInUse()  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _count:=%d. valid=%s, inUse=%d",           tuple.m_count.get(), b2t_(tuple.m_count.isValid()), tuple.m_count.isInUse() ));
    }


static void traceBar1_( Point::Bar1& point, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("==> %s::Bar1: %s", name, msg ));
    traceFoo1_( point.m_tuple, name, "" );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("") );
    }

static void traceBar2_( Point::Bar2& point, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("==> %s::Bar2: %s", name, msg ));
    traceFoo1_( point.m_foo1, name, "" );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("") );
    traceFoo2_( point.m_foo2, name, "" );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("") );
    }


static void traceBar3_( Point::Bar3& point, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("==> %s::Bar3: %s", name, msg ));
    unsigned i;
    Cpl::Text::FString<2> idx;
    for(i=0; i<point.getNumTuples(); i++)
        {                    
        idx = i;
        traceFoo3_( point[i], name, idx );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("") );
        }
    }


////////////////////////////////////////////
class ViewerContext: public Cpl::Itc::CloseSync
{
public: // Make public to simply testing
    ///    
    Point::ViewerBar1<ViewerContext>   m_bar1;
    ///
    Point::ViewerBar2<ViewerContext>   m_bar2;
    ///
    Point::ViewerBar3<ViewerContext>   m_bar3;
    ///
    int                                m_openViewerCount;
    ///
    Cpl::Itc::CloseRequest::CloseMsg*  m_closeMsgPtr;
    ///
    const char*                        m_name;

public:
    ///
    ViewerContext( const char*          viewerName,
                   Cpl::Itc::PostApi&   viewerMbox, 
                   Point::ModelBar1&    modelBar1,
                   Point::ModelBar2&    modelBar2,
                   Point::ModelBar3&    modelBar3,
                   bool                 enableAllInUseBar1 = true,
                   bool                 enableAllInUseBar2 = true,
                   bool                 enableAllInUseBar3 = true
                 )
        :CloseSync(viewerMbox)
        ,m_bar1(*this,
                &ViewerContext::bar1Changed,
                &ViewerContext::bar1Stopped,
                modelBar1,
                viewerMbox
               )
        ,m_bar2(*this,
                &ViewerContext::bar2Changed,
                &ViewerContext::bar2Stopped,
                modelBar2,
                viewerMbox
               )
        ,m_bar3(*this,
                &ViewerContext::bar3Changed,
                &ViewerContext::bar3ContainerChanged,
                &ViewerContext::bar3Stopped,
                modelBar3,
                viewerMbox
               )
        ,m_openViewerCount(0)
        ,m_closeMsgPtr(0)
        ,m_name(viewerName)
            {
            if ( enableAllInUseBar1 )
                {
                m_bar1.setAllInUseState(true);
                }
            if ( enableAllInUseBar2 )
                {
                m_bar2.setAllInUseState(true);
                }
            if ( enableAllInUseBar3 )
                {
                m_bar3.setAllInUseState(true);
                }
            }

private:
    ///
    void viewerStopped(void)
        {
        if ( --m_openViewerCount == 0 )
            {
            if ( m_closeMsgPtr )
                {
                m_closeMsgPtr->returnToSender();
                }
            }
        }

protected:
    ///
    void bar1Changed(void)
        {
        traceBar1_( m_bar1, m_name, "Viewer::Changed" );
        }
    ///
    void bar1Stopped(void)
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Viewer(%s).Bar1 STOPPED", m_name ));
        viewerStopped();
        }

    ///
    void bar2Changed(void)
        {
        traceBar2_( m_bar2, m_name, "Viewer::Changed" );
        }
    ///
    void bar2Stopped(void)
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Viewer(%s).Bar2 STOPPED", m_name ));
        viewerStopped();
        }

    ///
    void bar3Changed(void)
        {
        traceBar3_( m_bar3, m_name, "Viewer::Tuple Changed" );
        }
    void bar3ContainerChanged(void)
        {
        traceBar3_( m_bar3, m_name, "Viewer::CONTAINER Changed" );
        }
    void bar3Stopped(void)
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Viewer(%s).Bar3 STOPPED", m_name ));
        viewerStopped();
        }


protected: // Cpl::Itc::Close/Open
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "ViewerContext::request( OpenMsg ): starting viewers" ));
        m_openViewerCount += m_bar1.startViewing();
        m_openViewerCount += m_bar2.startViewing();
        m_openViewerCount += m_bar3.startViewing();
        msg.returnToSender();
        }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "ViewerContext::request( CloseMsg ): stopping viewers" ));
        m_bar1.stopViewing();
        m_bar2.stopViewing();
        m_bar3.stopViewing();
        m_closeMsgPtr      = &msg;        
        }

};


///////////////////////////////////////////////////////////////////

// Mailbox for the server and viewer threads
static Cpl::Itc::MailboxServer modelMailbox_;
static Cpl::Itc::MailboxServer viewerMailbox_;

// Model Points
static  Point::ModelBar1 modelBar1_(modelMailbox_);
static  Point::ModelBar2 modelBar2_(modelMailbox_);
static  Point::ModelBar3 modelBar3_(modelMailbox_);




#endif // end header latch
