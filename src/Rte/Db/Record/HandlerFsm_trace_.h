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

/* Command line options: -p CADIFRA -doxygen -o HandlerFsm -l cppx -Trace HandlerFsm.cdd   */
/* This file is generated from HandlerFsm_trace - do not edit manually*/
/* Generated on: version 3.6.9 */


#ifndef __HANDLERFSM_TRACE_RTE_DB_RECORD_H__
#define __HANDLERFSM_TRACE_RTE_DB_RECORD_H__

#include "Cpl/System/Trace.h"

#define SECT_ "Rte::Db::Record::HandlerFsm"


namespace Rte { namespace Db { namespace Record  {



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


};};}; //namespace Rte { namespace Db { namespace Record 


#endif
