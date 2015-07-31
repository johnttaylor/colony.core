#ifndef Rte_Db_Record_FsmEventQueue_x_h_
#define Rte_Db_Record_FsmEventQueue_x_h_
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



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


#include "Rte/Db/Record/Fsm_.h"
#include "Cpl/Container/RingBuffer.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Record { 


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

};};};  /// end namespace(s)
#endif /// end header latch
