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

/* Command line options: -p CADIFRA -doxygen -o Fsm -l cppx -Trace Fsm.cdd   */
/* This file is generated from Fsm_trace - do not edit manually  */
/* Generated on: version 3.7.4b3 */ 


#ifndef __FSM_TRACE_CPL_DM_PERSISTENCE_RECORD_H__
#define __FSM_TRACE_CPL_DM_PERSISTENCE_RECORD_H__

#include "Cpl/System/Trace.h"

/// Trace Section
#define SECT_ "Cpl::Dm::Persistence::Record::Fsm"


#ifndef DOXYGEN_WILL_SKIP_THIS

namespace Cpl { namespace Dm { namespace Persistence { namespace Record  {


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


}}}} //namespace Cpl { namespace Dm { namespace Persistence { namespace Record 


#endif // !DOXYGEN_WILL_SKIP_THIS

#endif
