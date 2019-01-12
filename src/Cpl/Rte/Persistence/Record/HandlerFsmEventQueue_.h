#ifndef Cpl_Rte_Persistence_Record_HandlerFsmEventQueue_x_h_
#define Cpl_Rte_Persistence_Record_HandlerFsmEventQueue_x_h_
/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


#include "Cpl/Rte/Persistence/Record/HandlerFsm_.h"
#include "Cpl/Container/RingBuffer.h"


/// Namespace(s)
namespace Cpl { namespace Rte { namespace Persistence { namespace Record { 


/// Event Queue for FSM events.
class HandlerFsmEventQueue_: public HandlerFsm, public Cpl::Container::RingBuffer<HANDLERFSM_EVENT_T>
{
protected:
    /// Memory for Event queue
    HANDLERFSM_EVENT_T m_eventQueMemory[4];

    /// Flag for tracking re-entrant events
    bool        m_processingFsmEvent;

public:
    /// Constructor
    HandlerFsmEventQueue_();

public:
    /// This method properly queues and process event messages
    void generateEvent( HANDLERFSM_EVENT_T msg );
};

};};};};  /// end namespace(s)
#endif /// end header latch