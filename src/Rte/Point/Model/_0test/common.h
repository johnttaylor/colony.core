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

#include "Rte/Point/Model/_0test/Point/bar1_m.h"   // Note: violating #include rule about not including from sub-directories -->since this is test code I rationalize the cheating!
#include "Rte/Point/Model/_0test/Point/bar1_c.h"
#include "Rte/Point/Model/_0test/Point/bar1_q.h"

#include "Rte/Point/Model/_0test/Point/bar2_m.h"
#include "Rte/Point/Model/_0test/Point/bar2_c.h"
#include "Rte/Point/Model/_0test/Point/bar2_q.h"

#include "Rte/Point/Model/_0test/Point/bar3_m.h"
#include "Rte/Point/Model/_0test/Point/bar3_c.h"
#include "Rte/Point/Model/_0test/Point/bar3_q.h"


#define SECT_           "_0test"



// Mailbox for the server and viewer threads
static Cpl::Itc::MailboxServer modelMailbox_;
static Cpl::Itc::MailboxServer viewerMailbox_;

// Model Points
static  Point::ModelBar1 modelBar1_(modelMailbox_);
static  Point::ModelBar2 modelBar2_(modelMailbox_);
static  Point::ModelBar3 modelBar3_(modelMailbox_);


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
    CPL_SYSTEM_TRACE_MSG( SECT_, ("    #   _inContainer_:=[%s]. valid=%s, inUse=%d",  b2t_(tuple.m_inContainer_.get()), b2t_(tuple.m_inContainer_.isValid()), tuple.m_inContainer_.isInUse()  ));
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
    for(i=0; i<point.getNumTuples(); i++)
        {
        traceFoo3_( point[i], name, "" );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("") );
        }
    }



#endif // end header latch
