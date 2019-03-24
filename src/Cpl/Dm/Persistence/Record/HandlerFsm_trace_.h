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

/* Command line options: -p CADIFRA -doxygen -o HandlerFsm -l cppx -Trace HandlerFsm.cdd   */
/* This file is generated from HandlerFsm_trace - do not edit manually  */
/* Generated on: version 3.7.4b3 */ 


#ifndef __HANDLERFSM_TRACE_CPL_DM_PERSISTENCE_RECORD_H__
#define __HANDLERFSM_TRACE_CPL_DM_PERSISTENCE_RECORD_H__

#include "Cpl/System/Trace.h"

/// Trace Section
#define SECT_ "Cpl::Dm::Persistence::Record::HandlerFsm"


#ifndef DOXYGEN_WILL_SKIP_THIS

namespace Cpl { namespace Dm { namespace Persistence { namespace Record  {


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
    "HandlerFsm_evResponse[isFileError()]",
    "HandlerFsm_evResponse[isFileSuccess()]",
    "HandlerFsm_evResponse[isFileBadData()]",
    "HandlerFsm_evResponse[isFileEof()]",
    "HandlerFsm_evResponse[!isFileSuccess()]",
    "HandlerFsm_evResponse[!isFileError()]",
    "HandlerFsm_evResponse[isNotCompatible()]"
};


}}}} //namespace Cpl { namespace Dm { namespace Persistence { namespace Record 


#endif // !DOXYGEN_WILL_SKIP_THIS

#endif
