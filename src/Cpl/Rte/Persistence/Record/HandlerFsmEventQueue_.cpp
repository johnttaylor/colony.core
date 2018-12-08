/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


#include "HandlerFsmEventQueue_.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"

#define SECT_ "Cpl::Rte::Persistence::Record::HandlerFsm"

/// Namespace(s)
namespace Cpl { namespace Rte { namespace Persistence { namespace Record { 


HandlerFsmEventQueue_::HandlerFsmEventQueue_()
:Cpl::Container::RingBuffer<HANDLERFSM_EVENT_T>( 4, m_eventQueMemory )
,m_processingFsmEvent(false)
    {
    }


void HandlerFsmEventQueue_::generateEvent( HANDLERFSM_EVENT_T msg )
    {
    // Queue my event
    if ( !add( msg ) )
        {
        Cpl::System::FatalError::logf( "Cpl::Rte::Persistence::Record::HandlerFsmEventQueue_: - Buffer Overflow!" );
        }

    // Protect against in-thread 'feedback loops' that can potentially generate events
    if ( !m_processingFsmEvent )
        {
        m_processingFsmEvent = true;
        while( remove( msg ) )
            {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Processing event:= %s, current state=%s ...", getNameByEvent(msg), getNameByState(getInnermostActiveState())) );
            if ( processEvent(msg) == 0 )
                {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("  Event IGNORED:= %s", getNameByEvent(msg)) );
                }
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Event Completed:=  %s, end state=%s", getNameByEvent(msg), getNameByState(getInnermostActiveState())) );
            }

        m_processingFsmEvent = false;
        }
    }

};};};};  /// end namespace(s)
