/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -doxygen -o Fsm -l cppx -Trace Fsm.cdd   */
/* This file is generated from Fsm.cdd - do not edit manually  */
/* Generated on:  version 3.7 */


#ifndef __FSM_CPL_RTE_PERSISTENCE_RECORD_EXT_H__
#define __FSM_CPL_RTE_PERSISTENCE_RECORD_EXT_H__

// Events which can be sent to the state-machine

namespace Cpl { namespace Dm { namespace Persistence { namespace Record  {

enum FSM_EVENT_T {
    Fsm_evLoadDone=0U,
    Fsm_evDefault,
    Fsm_evTimerExpired,
    Fsm_evStart,
    Fsm_evDataModified,
    Fsm_evReadDone,
    Fsm_evStop,
    Fsm_evWriteDone,
    FSM_NO_MSG
};


};};};}; //namespace Cpl { namespace Dm { namespace Persistence { namespace Record 


#endif
