/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -doxygen -o HandlerFsm -l cppx -Trace HandlerFsm.cdd   */
/* This file is generated from HandlerFsm_trace - do not edit manually  */
/* Generated on: version 3.7 */ 


#ifndef __HANDLERFSM_TRACE_CPL_RTE_PERSISTENCE_RECORD_H__
#define __HANDLERFSM_TRACE_CPL_RTE_PERSISTENCE_RECORD_H__

#include "Cpl/System/Trace.h"

#define SECT_ "Cpl::Rte::Persistence::Record::HandlerFsm"


namespace Cpl { namespace Rte { namespace Persistence { namespace Record  {


/*Simulation which can be sent to the state-machine statediagram.simulator  */

#define HandlerFsmTraceEvent(a) CPL_SYSTEM_TRACE_MSG( SECT_, ( "Old State=%s, Event=%s", getNameByState(getInnermostActiveState()), HandlerFsmTraceEvents[a] ));

const char* const HandlerFsmTraceEvents[] = {
    "HandlerFsm_evStart",
    "HandlerFsm_evStop",
    "HandlerFsm_evWrite",
    "HandlerFsm_evStopped",
    "",
    "HandlerFsm_evVerified",
    "HandlerFsm_evIncompleteLoad",
    "HandlerFsm_evResponse[isDbError()]",
    "HandlerFsm_evResponse[isDbSuccess()]",
    "HandlerFsm_evResponse[isDbBadData()]",
    "HandlerFsm_evResponse[isDbEof()]",
    "HandlerFsm_evResponse[!isDbSuccess()]",
    "HandlerFsm_evResponse[!isDbError()]",
    "HandlerFsm_evResponse[isNotCompatible()]"
};


};};};}; //namespace Cpl { namespace Rte { namespace Persistence { namespace Record 


#endif
