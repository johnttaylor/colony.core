#ifndef Cpl_Type_FsmEventQueue_h_
#define Cpl_Type_FsmEventQueue_h_
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

#include "Cpl/Container/RingBuffer.h"
#include "Cpl/System/FatalError.h"


/// 
namespace Cpl { namespace Type { 

/** This tempalte class defines an Event Queue to be used with a Finite State 
    Machine (FSM). The purpse of the event queue is to ensure run-to-completion 
    sematics on its event processing. 

    Template args:
        FSM     - The FSM class that contains the actual process event message
                  method. 
        MSG_T   - Event message type of the FSM
        DEPTH   - The depth of the event queue
 */
template <class FSM, class MSG_T, int DEPTH=4>
class FsmEventQueue: public Cpl::Container::RingBuffer<MSG_T>
{
public:         
    /** Definition of funciton pointer that process the event message.
        This method returns a non-zero value if the message was consumed; else
        zero is returned when the mesage is 'ignored'
     */
    typedef int (FSM::* ProcessEventFunction_T)( MSG_T );


protected:
    /// FSM context
    FSM&                    m_context;

    /// Function Pointer
    ProcessEventFunction_T  m_func;

    /// Memory for my ringbuffer
    MSG_T                   m_memory[DEPTH];
        
    /// Flag for tracking re-entry events
    bool                    m_processingFsmEvent;

public:
    /// Constructor
    FsmEventQueue( FSM& fsm, ProcessEventFunction_T functionPtr )
        :Cpl::Container::RingBuffer<MSG_T>( DEPTH, m_memory )
        ,m_context( fsm )
        ,m_func( functionPtr )
        ,m_processingFsmEvent( false )
            {
            }

public:
    /** This method properly queues and process event messages
     */
    void generateEvent( MSG_T event )
        {
        // Queue my event
        if ( !add( event ) )
            {
            Cpl::System::FatalError::logf( "Cpl::Type::FsmEventQueue: - Buffer Overflow!" );
            }

        // Protect against in-thread 'feedback loops' that can potentially generate events
        if ( !m_processingFsmEvent )
            {
            m_processingFsmEvent = true;
            while( remove( event ) )
                {
                (m_context.*m_func)(event);
                }

            m_processingFsmEvent = false;
            }
        }

};



};      // end namespaces
};
#endif  // end header latch

