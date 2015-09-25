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
#include "Cpl/Text/atob.h"

#include "Cpl/Itc/CloseSync.h"

#include "Rte/TShell/Dac/_0test/Point/bar1_m.h"   // Note: violating #include rule about not including from sub-directories -->since this is test code I rationalize the cheating!
#include "Rte/TShell/Dac/_0test/Point/bar1_c.h"
#include "Rte/TShell/Dac/_0test/Point/bar1_q.h"
#include "Rte/TShell/Dac/_0test/Point/bar1_v.h"

#include "Rte/TShell/Dac/_0test/Point/bar2_m.h"
#include "Rte/TShell/Dac/_0test/Point/bar2_c.h"
#include "Rte/TShell/Dac/_0test/Point/bar2_q.h"
#include "Rte/TShell/Dac/_0test/Point/bar2_v.h"

#include "Rte/TShell/Dac/_0test/Point/bar3_m.h"
#include "Rte/TShell/Dac/_0test/Point/bar3_c.h"
#include "Rte/TShell/Dac/_0test/Point/bar3_q.h"
#include "Rte/TShell/Dac/_0test/Point/bar3_v.h"

#include "Cpl/TShell/Dac/Cmd/Command.h"



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

static const char* n2s_( const char* elementValue, bool isValid )
    {
    if ( isValid )
        {
        return elementValue;
        }
    else
        {
        return "<?>";
        }
    }


static void traceFoo1_( Tuple::Foo1& tuple, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    # %s::Foo1: %s", name, msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _name:=[%s]. valid=%s, inUse=%s, isLocked=%s",  n2s_(tuple.m_name.get(), tuple.m_name.isValid()), v2t_(tuple.m_name.validState()), b2t_(tuple.m_name.isInUse()), b2t_(tuple.m_name.isLocked())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _enabled:=%d. valid=%s, inUse=%s, isLocked=%s", tuple.m_enabled.get(), v2t_(tuple.m_enabled.validState()), b2t_(tuple.m_enabled.isInUse()), b2t_(tuple.m_enabled.isLocked())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _count:=%d. valid=%s, inUse=%s, isLocked=%s",   tuple.m_count.get(), v2t_(tuple.m_count.validState()), b2t_(tuple.m_count.isInUse()), b2t_(tuple.m_count.isLocked()) ));
    }

static void traceFoo2_( Tuple::Foo2& tuple, const char* name, const char* msg )
    {
    Cpl::Text::FString<32> temp;
    tuple.m_checksum.toString(temp);

    CPL_SYSTEM_TRACE_MSG( SECT_, ("    # %s::Foo2: %s", name, msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _flags:=0x%lX. valid=%s, inUse=%s, isLocked=%s", tuple.m_flags.get(), v2t_(tuple.m_flags.validState()), b2t_(tuple.m_flags.isInUse()), b2t_(tuple.m_flags.isLocked())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _seqNum:=%ld. valid=%s, inUse=%s, isLocked=%s",tuple.m_seqNum.get(), v2t_(tuple.m_seqNum.validState()), b2t_(tuple.m_seqNum.isInUse()), b2t_(tuple.m_seqNum.isLocked()) ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _time:=%lld. valid=%s, inUse=%s, isLocked=%s",  (long long)tuple.m_time.get(), v2t_(tuple.m_time.validState()), b2t_(tuple.m_time.isInUse()), b2t_(tuple.m_time.isLocked()) ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _md5:=0x%s. valid=%s, inUse=%s, isLocked=%s",   temp(), v2t_(tuple.m_checksum.validState()), b2t_(tuple.m_checksum.isInUse()), b2t_(tuple.m_checksum.isLocked()) ));
    }

static void traceFoo3_( Tuple::Foo3& tuple, const char* name, const char* msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    # %s::Foo3: %s", name, msg ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _inContainer_:=%s. valid=%s, inUse=%s, isLocked=%s",    b2t_(tuple.isInContainer()), v2t_(tuple.m_inContainer_.validState()), b2t_(tuple.m_inContainer_.isInUse()), b2t_(tuple.m_inContainer_.isLocked())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _name:=[%s]. valid=%s, inUse=%s, isLocked=%s",          n2s_(tuple.m_name.get(), tuple.m_name.isValid()), v2t_(tuple.m_name.validState()), b2t_(tuple.m_name.isInUse()), b2t_(tuple.m_name.isLocked())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _enabled:=%d. valid=%s, inUse=%s, isLocked=%s",         tuple.m_enabled.get(), v2t_(tuple.m_enabled.validState()), b2t_(tuple.m_enabled.isInUse()), b2t_(tuple.m_enabled.isLocked())  ));
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _count:=%d. valid=%s, inUse=%s, isLocked=%s",           tuple.m_count.get(), v2t_(tuple.m_count.validState()), b2t_(tuple.m_count.isInUse()), b2t_(tuple.m_count.isLocked()) ));
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
class ViewerApi
{
public:
    ///
    virtual unsigned getPoint1Count(void) const = 0;
    ///
    virtual unsigned getPoint2Count(void) const = 0;
    ///
    virtual unsigned getPoint3Count(void) const = 0;
    ///
    virtual unsigned getPoint3MembershipCount(void) const = 0;
    ///
    virtual void clearCounts(void) = 0;

public:
    ///
    virtual ~ViewerApi(){}
};


class ViewerContext: public ViewerApi,
                     public Cpl::Itc::CloseSync
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
        m_openViewerCount += m_bar1.startViewing( m_useValueCompare1, true );
        m_openViewerCount += m_bar2.startViewing( m_useValueCompare2, true );
        m_openViewerCount += m_bar3.startViewing( m_useValueCompare3, true );
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


public:
    ///
    unsigned getPoint1Count(void) const
        {
        return m_changed1Count;
        }
    ///
    unsigned getPoint2Count(void) const
        {
        return m_changed2Count;
        }
    ///
    unsigned getPoint3Count(void) const
        {
        return m_changed3Count;
        }
    ///
    unsigned getPoint3MembershipCount(void) const
        {
        return m_membershipChanged3Count;
        }
    ///
    void clearCounts(void)
        {
        m_changed1Count           = 0;
        m_changed2Count           = 0;
        m_changed3Count           = 0;
        m_membershipChanged3Count = 0;
        }
};

class LWViewerContext: public ViewerApi,
                       public Cpl::Itc::CloseSync
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
    unsigned                           m_changed2Count;
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
        ,m_changed2Count(0)
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
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "LW-Viewer(%s).Bar1 Changed", m_name ));

        Point::QueryBar1 queryResult( m_bar1.getModelPoint() );
        queryResult.issueQuery();
        m_bar1.copyQueryResultsFrom( queryResult );
        
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "V4 Viewer::Bar1 SeqNum=%u, query.SeqNum=%u, count=%u", m_bar1.m_tuple.getSequenceNumber(), queryResult.m_tuple.getSequenceNumber(), m_changed1Count ));

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
        
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "LW-Viewer(%s).Bar3 Changed", m_name ));
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

        CPL_SYSTEM_TRACE_MSG( SECT_, ( "V4 Viewer::Bar3 count=%u, membershipCount=%u", m_changed3Count, m_membershipChanged3Count ));
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
        m_openViewerCount += m_bar1.startViewing();
        m_openViewerCount += m_bar3.startViewing();
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
            m_bar3.stopViewing();
            m_closeMsgPtr = &msg;        
            }
        }

public:
    ///
    unsigned getPoint1Count(void) const
        {
        return m_changed1Count;
        }
    ///
    unsigned getPoint2Count(void) const
        {
        return m_changed2Count;
        }
    ///
    unsigned getPoint3Count(void) const
        {
        return m_changed3Count;
        }
    ///
    unsigned getPoint3MembershipCount(void) const
        {
        return m_membershipChanged3Count;
        }
    ///
    void clearCounts(void)
        {
        m_changed1Count           = 0;
        m_changed2Count           = 0;
        m_changed3Count           = 0;
        m_membershipChanged3Count = 0;
        }
};



    
///////////////////////////////////////////////////////////////////
class Viewers: public Cpl::TShell::Dac::Cmd::Command
{
public:
    /// See Cpl::TShell::Dac::Command
    const char* getUsage() const throw()   { return "viewers\nviewers <num> [clear]\nviewers clear"; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const throw()    { return "  Displays the Model viewer counts and optionally clears the counts."; }
    
    ///
    static const unsigned MAX_VIEWERS = 4;

    /// Viewers
    ViewerApi*  m_viewers[MAX_VIEWERS];

     
public:
    /// Constructor
    Viewers( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, 
             ViewerApi*                                      viewer0 = 0,
             ViewerApi*                                      viewer1 = 0,
             ViewerApi*                                      viewer2 = 0,
             ViewerApi*                                      viewer3 = 0
           )
    :Command(commandList, "viewers")
        {
        m_viewers[0] = viewer0;
        m_viewers[1] = viewer1;
        m_viewers[2] = viewer2;
        m_viewers[3] = viewer3;
        }


    /// Constructor
    Viewers( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, 
             const char*                                     ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance, 
             ViewerApi*                                      viewer0 = 0,
             ViewerApi*                                      viewer1 = 0,
             ViewerApi*                                      viewer2 = 0,
             ViewerApi*                                      viewer3 = 0
           )
    :Command(commandList, "viewers", ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
        {
        m_viewers[0] = viewer0;
        m_viewers[1] = viewer1;
        m_viewers[2] = viewer2;
        m_viewers[3] = viewer3;
        }


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
        {
        Cpl::TShell::Dac::ActiveVariablesApi& vars    = context.getVariables();
        Cpl::Text::String&                    outtext = context.getOutputBuffer();
        Cpl::Text::String&                    token   = context.getTokenBuffer();
        bool                                  io      = true;

        // Error checking
        if ( tokens.numParameters() > 3 )
            {
            return Command::eERROR_EXTRA_ARGS;
            }

        // Display ALL viewers' count
        if ( tokens.numParameters() == 1 )
            {
            unsigned i;
            for( i=0; io && i<MAX_VIEWERS; i++ )
                {
                io &= displayCounts( i, context, vars, outtext );
                }

            return io? Command::eSUCCESS: Command::eERROR_IO;
            }
             
        // Clear all viewers
        if ( tokens.numParameters() == 2 && strcmp(tokens.getParameter(1),"clear") == 0 )
            {
            unsigned i;
            for( i=0; io && i<MAX_VIEWERS; i++ )
                {
                ViewerApi* ptr = m_viewers[i];
                if ( ptr )
                    {
                    ptr->clearCounts();
                    }
                }

            return Command::eSUCCESS;
            }

        // Get viewer number
        unsigned viewerNum = 0;
        if ( expandText( tokens.getParameter(1), token, vars ) != Command::eSUCCESS  || !Cpl::Text::a2ui(viewerNum, token) )
            {
            return Command::eERROR_INVALID_ARGS;
            }

        // Display and individual viewer's count
        if ( tokens.numParameters() == 2 )
            {
            // Display counts
            return displayCounts( viewerNum, context, vars, outtext )? Command::eSUCCESS: Command::eERROR_IO;
            }

        // Clear counts
        ViewerApi* ptr = m_viewers[viewerNum];
        if ( ptr )
            {
            ptr->clearCounts();
            }
        return Command::eSUCCESS;
        }


    ///    
    bool displayCounts( unsigned idx, Cpl::TShell::Dac::Context_& context, Cpl::TShell::Dac::ActiveVariablesApi& vars, Cpl::Text::String& outtext )
        {
        ViewerApi* ptr = m_viewers[idx];
        if ( ptr )
            {
            outtext.format( "viewer: %02d  Change counts: %02d %02d %02d  Membership3 Count: %02d",
                            idx, 
                            ptr->getPoint1Count(),
                            ptr->getPoint2Count(),
                            ptr->getPoint3Count(),
                            ptr->getPoint3MembershipCount()
                          );
            return context.writeFrame( outtext );
            }

        return true;
        }

};


#endif // end header latch
