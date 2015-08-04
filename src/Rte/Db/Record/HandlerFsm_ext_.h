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
/* This file is generated from HandlerFsm.cdd - do not edit manually  */
/* Generated on:  version 3.6.9 */


#ifndef __HANDLERFSM_RTE_DB_RECORD_EXT_H__
#define __HANDLERFSM_RTE_DB_RECORD_EXT_H__

// Events which can be sent to the state-machine

namespace Rte { namespace Db { namespace Record  {

enum HANDLERFSM_EVENT_T {
    HandlerFsm_evWrite=0U,
    HandlerFsm_evStop,
    HandlerFsm_evVerified,
    HandlerFsm_evResponse,
    HandlerFsm_evIncompleteLoad,
    HandlerFsm_evStopped,
    HandlerFsm_evStart,
    HANDLERFSM_NO_MSG
};


};};}; //namespace Rte { namespace Db { namespace Record 


#endif
