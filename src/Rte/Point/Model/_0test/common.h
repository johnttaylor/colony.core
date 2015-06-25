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

static const char* v2t_( int8_t valid_state )
    {
    static Cpl::Text::FString<4> validstring;
   
    validstring = valid_state;
    if ( valid_state == RTE_ELEMENT_API_STATE_VALID )
        {
        validstring = "YES";
        }

    return validstring;
    }

static void traceFoo1_( Tuple::Foo1& tuple, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    # %s::Foo1: %s", name, msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _name:=[%s]. valid=%s, inUse=%s",  tuple.m_name.get(), v2t_(tuple.m_name.isValid()), b2t_(tuple.m_name.isInUse())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _enabled:=%d. valid=%s, inUse=%s", tuple.m_enabled.get(), v2t_(tuple.m_enabled.isValid()), b2t_(tuple.m_enabled.isInUse())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _count:=%d. valid=%s, inUse=%s",   tuple.m_count.get(), v2t_(tuple.m_count.isValid()), b2t_(tuple.m_count.isInUse()) ));
    }

static void traceFoo2_( Tuple::Foo2& tuple, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    # %s::Foo2: %s", name, msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _enabled:=%d. valid=%s, inUse=%s", tuple.m_enabled.get(), v2t_(tuple.m_enabled.isValid()), b2t_(tuple.m_enabled.isInUse())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _limit:=%d. valid=%s, inUse=%s",   tuple.m_limit.get(), v2t_(tuple.m_limit.isValid()), b2t_(tuple.m_limit.isInUse()) ));
    }

static void traceFoo3_( Tuple::Foo3& tuple, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    # %s::Foo3: %s", name, msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _inContainer_:=%s. valid=%s, inUse=%s",    b2t_(tuple.m_inContainer_.get()), v2t_(tuple.m_inContainer_.isValid()), b2t_(tuple.m_inContainer_.isInUse())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _name:=[%s]. valid=%s, inUse=%s",          tuple.m_name.get(), v2t_(tuple.m_name.isValid()), b2t_(tuple.m_name.isInUse())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _enabled:=%d. valid=%s, inUse=%s",         tuple.m_enabled.get(), v2t_(tuple.m_enabled.isValid()), b2t_(tuple.m_enabled.isInUse())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _count:=%d. valid=%s, inUse=%s",           tuple.m_count.get(), v2t_(tuple.m_count.isValid()), b2t_(tuple.m_count.isInUse()) ));
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
        traceFoo3_( point[i], name, idx() );
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
    ///
    unsigned                           m_changed1Count;
    ///
    unsigned                           m_changed2Count;
    ///
    unsigned                           m_changed3Count;
    ///
    unsigned                           m_membershipChanged3Count;
    ///
    bool                               m_useValueCompare1;
    ///
    bool                               m_useValueCompare2;
    ///
    bool                               m_useValueCompare3;


public:
    ///
    ViewerContext( const char*          viewerName,
                   Cpl::Itc::PostApi&   viewerMbox, 
                   Point::ModelBar1&    modelBar1,
                   Point::ModelBar2&    modelBar2,
                   Point::ModelBar3&    modelBar3,
                   bool                 enableAllInUseBar1  = true,
                   bool                 enableAllInUseBar2  = true,
                   bool                 enableAllInUseBar3  = true,
                   bool                 useValueCompareBar1 = true,
                   bool                 useValueCompareBar2 = true,
                   bool                 useValueCompareBar3 = true
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
                &ViewerContext::bar3Stopped,
                modelBar3,
                viewerMbox
               )
        ,m_openViewerCount(0)
        ,m_closeMsgPtr(0)
        ,m_name(viewerName)
        ,m_changed1Count(0)
        ,m_changed2Count(0)
        ,m_changed3Count(0)
        ,m_membershipChanged3Count(0)
        ,m_useValueCompare1(useValueCompareBar1)
        ,m_useValueCompare2(useValueCompareBar2)
        ,m_useValueCompare3(useValueCompareBar3)
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
        m_changed1Count++;
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
        m_changed2Count++;
        traceBar2_( m_bar2, m_name, "Viewer::Changed" );
        }
    ///
    void bar2Stopped(void)
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Viewer(%s).Bar2 STOPPED", m_name ));
        viewerStopped();
        }

    ///
    void bar3Changed( bool membershipChanged )
        {
        m_changed3Count++;

        if ( !membershipChanged )
            {
            traceBar3_( m_bar3, m_name, "Viewer::Tuple Changed" );
            }
        else
            {
            m_membershipChanged3Count++;
            traceBar3_( m_bar3, m_name, "Viewer::CONTAINER Changed" );
            }
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
        m_openViewerCount += m_bar1.startViewing(true, m_useValueCompare1);
        m_openViewerCount += m_bar2.startViewing(true, m_useValueCompare2);
        m_openViewerCount += m_bar3.startViewing(true, m_useValueCompare3);
        msg.returnToSender();
        }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "ViewerContext::request( CloseMsg ): stopping viewers" ));
        if ( m_openViewerCount == 0 )
            {
            msg.returnToSender();
            }
        else
            {
            m_bar1.stopViewing();
            m_bar2.stopViewing();
            m_bar3.stopViewing();
            m_closeMsgPtr = &msg;        
            }
        }

};

class LWViewerContext: public Cpl::Itc::CloseSync
{
public: // Make public to simply testing
    ///    
    Point::LWViewerBar1<LWViewerContext>   m_bar1;
    ///
    Point::LWViewerBar3<LWViewerContext>   m_bar3;
    ///
    int                                m_openViewerCount;
    ///
    Cpl::Itc::CloseRequest::CloseMsg*  m_closeMsgPtr;
    ///
    const char*                        m_name;
    ///
    unsigned                           m_changed1Count;
    ///
    unsigned                           m_changed3Count;
    ///
    unsigned                           m_membershipChanged3Count;


public:
    ///
    LWViewerContext( const char*          viewerName,
                     Cpl::Itc::PostApi&   viewerMbox, 
                     Point::ModelBar1&    modelBar1,
                     Point::ModelBar3&    modelBar3,
                     bool                 enableAllInUseBar1  = true,
                     bool                 enableAllInUseBar3  = true
                   )
        :CloseSync(viewerMbox)
        ,m_bar1(*this,
                &LWViewerContext::bar1Changed,
                &LWViewerContext::bar1Stopped,
                modelBar1,
                viewerMbox
               )
        ,m_bar3(*this,
                &LWViewerContext::bar3Changed,
                &LWViewerContext::bar3Stopped,
                modelBar3,
                viewerMbox
               )
        ,m_openViewerCount(0)
        ,m_closeMsgPtr(0)
        ,m_name(viewerName)
        ,m_changed1Count(0)
        ,m_changed3Count(0)
        ,m_membershipChanged3Count(0)
            {
            if ( enableAllInUseBar1 )
                {
                m_bar1.setAllInUseState(true);
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
        m_changed1Count++;

        Point::QueryBar1 queryResult( m_bar1.getModelPoint() );
        queryResult.issueQuery();
        m_bar1.copyQueryResultsFrom( queryResult );
        
CPL_SYSTEM_TRACE_MSG( SECT_, ( "V4 Viewer: SeqNum=%u, query.SeqNum=%u", m_bar1.m_tuple.getSequenceNumber(), queryResult.m_tuple.getSequenceNumber() ));

        traceBar1_( queryResult, m_name, "LW-Viewer::Changed" );
        }
    ///
    void bar1Stopped(void)
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "LW-Viewer(%s).Bar1 STOPPED", m_name ));
        viewerStopped();
        }


    ///
    void bar3Changed( bool membershipChanged )
        {
        m_changed3Count++;
        
        Point::QueryBar3 queryResult( m_bar3.getModelPoint() );
        queryResult.issueQuery();
        m_bar3.copyQueryResultsFrom( queryResult );

        if ( !membershipChanged )
            {
            traceBar3_( queryResult, m_name, "LW-Viewer::Tuple Changed" );
            }
        else
            {
            m_membershipChanged3Count++;
            traceBar3_( queryResult, m_name, "LW-Viewer::CONTAINER Changed" );
            }
        }

    void bar3Stopped(void)
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "LW-Viewer(%s).Bar3 STOPPED", m_name ));
        viewerStopped();
        }


protected: // Cpl::Itc::Close/Open
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "LWViewerContext::request( OpenMsg ): starting viewers" ));
        m_openViewerCount += m_bar1.startViewing(true);
//        m_openViewerCount += m_bar3.startViewing(true);
        msg.returnToSender();
        }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "LWViewerContext::request( CloseMsg ): stopping viewers" ));
        if ( m_openViewerCount == 0 )
            {
            msg.returnToSender();
            }
        else
            {
            m_bar1.stopViewing();
//            m_bar3.stopViewing();
            m_closeMsgPtr = &msg;        
            }
        }

};




class Foo3TupleTraverserContext
{
public:
    ///
    unsigned m_abortIdx;
    /// 
    unsigned m_startIdx;
    ///
    unsigned m_lastIdx;
    ///
    Rte::Point::Query::TupleItem<Tuple::Foo3, Foo3TupleTraverserContext> m_tupleWalker;
    ///
    Tuple::Foo3 m_tuples[4];

    
public:
    ///
    Foo3TupleTraverserContext( Point::ModelBar3& modelPoint )
        :m_abortIdx(0xFF)
        ,m_startIdx(0xFF)                
        ,m_lastIdx(0xFF)
        ,m_tupleWalker( modelPoint, *this, &Foo3TupleTraverserContext::item )
            {}

public:
    ///
    Cpl::Type::Traverser::Status_T item( Tuple::Foo3& nextTuple, unsigned tupleIndex )
        {
        Cpl::Text::FString<3> idx = tupleIndex;
        traceFoo3_( nextTuple, "Foo3 Traversal", idx );

        // Capture index info for unittesting checks
        Rte::Tuple::Api::copy( m_tuples[tupleIndex], nextTuple );
        m_lastIdx = tupleIndex;
        if ( m_startIdx == 0xFF )
            {
            m_startIdx = tupleIndex;
            }
        if ( tupleIndex == m_abortIdx )
            {
            return Cpl::Type::Traverser::eABORT;
            }

        return Cpl::Type::Traverser::eCONTINUE;
        }
};


class Bar1RmwControllerContext
{
public:
    ///
    Point::RwmControllerBar1<Bar1RmwControllerContext> m_rmwPoint;
    ///
    uint32_t                                           m_adder;

public:
    ///
    Bar1RmwControllerContext( Point::ModelBar1& modelPoint, uint32_t adder )
        :m_rmwPoint( *this, &Bar1RmwControllerContext::modify, modelPoint )
        ,m_adder(adder)
            {
            }

public:
    ///
    void modify( Rte::Point::Api*& modifiedPoint )
        {
        // Set my Point all NOT in-use (so I can update one and only one Element)
        m_rmwPoint.setAllInUseState(false);

        // Modify my read value
        m_rmwPoint.m_tuple.m_count.setInUse();
        m_rmwPoint.m_tuple.m_count.set( m_rmwPoint.m_tuple.m_count.get() + m_adder );

        // For testing purpose attempt to modify m_name -->which WOULDN'T take since its in-use flag is false
        m_rmwPoint.m_tuple.m_name.set( "hello" );
 
        // Pass the Point to be used as the source for the write operation
        modifiedPoint = &m_rmwPoint;
        }
};


class Bar3RmwContainerContext
{
public:
    ///
    Rte::Point::Controller::RmwContainer<Tuple::Foo3,Bar3RmwContainerContext> m_rmwPoint;
    ///
    uint32_t m_adder;
    /// 
    unsigned m_sequence;

public:
    ///
    Bar3RmwContainerContext( Point::ModelBar3& modelPoint, uint32_t adder )
        :m_rmwPoint( *this, &Bar3RmwContainerContext::modifyItem, modelPoint, 3)
        ,m_adder(adder)
        ,m_sequence(1)
            {
            }



public:
    ///
    int modifyItem(unsigned tupleIndex, Rte::Tuple::Api*& modifiedTuplePtr, bool& membershipChanged )
        {
        if ( tupleIndex == 3 )
            {
            membershipChanged = m_rmwPoint.removeItem();
            m_rmwPoint.m_count.setInUse();
            m_rmwPoint.m_count.set( m_rmwPoint.m_count.get() + m_adder + m_sequence++ * 1000 ); 
            modifiedTuplePtr  = &m_rmwPoint;
            return 0;
            }
    
        if ( tupleIndex == 0 )
            {
            m_rmwPoint.m_count.setInUse();
            m_rmwPoint.m_count.set( m_rmwPoint.m_count.get() + m_adder + + m_sequence++ * 1000 );
            modifiedTuplePtr  = &m_rmwPoint;
            return Rte::Point::Controller::RmwContainerClient::eNEXT;
            }

        if ( tupleIndex == 1 )
            {                                      
            membershipChanged = m_rmwPoint.addItem();
            m_rmwPoint.m_count.setInUse();
            m_rmwPoint.m_count.set( m_rmwPoint.m_count.get() + m_adder + + m_sequence++ * 1000 );
            modifiedTuplePtr  = &m_rmwPoint;
            return Rte::Point::Controller::RmwContainerClient::eDONE;
            }

        // Should NEVER get here!!!!
        m_sequence++;
        return 100;
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
