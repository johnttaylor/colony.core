#ifndef Cpl_Rte_Persistence_Record_FsmEventQueue_x_h_
#define Cpl_Rte_Persistence_Record_FsmEventQueue_x_h_
/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


#include "Cpl/Rte/Persistence/Record/Fsm_.h"
#include "Cpl/Container/RingBuffer.h"


/// Namespace(s)
namespace Cpl { namespace Rte { namespace Persistence { namespace Record { 


/// Event Queue for FSM events.
class FsmEventQueue_: public Fsm, public Cpl::Container::RingBuffer<FSM_EVENT_T>
{
private:
    /// Memory for Event queue
    FSM_EVENT_T m_eventQueMemory[4];

    /// Flag for tracking re-entrant events
    bool        m_processingFsmEvent;

public:
    /// Constructor
    FsmEventQueue_();

public:
    /// This method properly queues and process event messages
    void generateEvent( FSM_EVENT_T msg );
};

};};};};  /// end namespace(s)
#endif /// end header latch
