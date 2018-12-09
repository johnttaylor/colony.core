/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -doxygen -o Fsm -l cppx -Trace Fsm.cdd   */
/* This file is generated from Fsm_trace - do not edit manually  */
/* Generated on: version 3.7 */ 


#ifndef __FSM_TRACE_CPL_RTE_PERSISTENCE_RECORD_H__
#define __FSM_TRACE_CPL_RTE_PERSISTENCE_RECORD_H__

#include "Cpl/System/Trace.h"

#define SECT_ "Cpl::Rte::Persistence::Record::Fsm"


namespace Cpl { namespace Rte { namespace Persistence { namespace Record  {


/*Simulation which can be sent to the state-machine statediagram.simulator  */

#define FsmTraceEvent(a) CPL_SYSTEM_TRACE_MSG( SECT_, ( "Old State=%s, Event=%s", getNameByState(getInnermostActiveState()), FsmTraceEvents[a] ));

const char* const FsmTraceEvents[] = {
    "Fsm_evStart",
    "Fsm_evStop",
    "Fsm_evReadDone",
    "Fsm_evLoadDone",
    "Fsm_evDefault",
    "Fsm_evWriteDone",
    "Fsm_evDataModified",
    "Fsm_evTimerExpired",
    "Fsm_evLoadDone[isLoadGood()]",
    "Fsm_evLoadDone[else]",
    "Fsm_evWriteDone[isDirty()]",
    "Fsm_evWriteDone[else]"
};


};};};}; //namespace Cpl { namespace Rte { namespace Persistence { namespace Record 


#endif
