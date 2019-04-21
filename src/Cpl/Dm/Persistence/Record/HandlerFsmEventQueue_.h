#ifndef Cpl_Dm_Persistence_Record_HandlerFsmEventQueue_x_h_
#define Cpl_Dm_Persistence_Record_HandlerFsmEventQueue_x_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


#include "Cpl/Dm/Persistence/Record/HandlerFsm_.h"
#include "Cpl/Container/RingBuffer.h"


/// Namespace(s)
namespace Cpl { namespace Dm { namespace Persistence { namespace Record { 


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
