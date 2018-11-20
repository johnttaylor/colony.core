/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -doxygen -o HandlerFsm -l cppx -Trace HandlerFsm.cdd   */
/* This file is generated from HandlerFsm.cdd - do not edit manually  */
/* Generated on:  version 3.7 */


#ifndef __HANDLERFSM_CPL_RTE_PERSISTENCE_RECORD_EXT_H__
#define __HANDLERFSM_CPL_RTE_PERSISTENCE_RECORD_EXT_H__

// Events which can be sent to the state-machine

namespace Cpl { namespace Rte { namespace Persistence { namespace Record  {

enum HANDLERFSM_EVENT_T {
    HandlerFsm_evIncompleteLoad=0U,
    HandlerFsm_evResponse,
    HandlerFsm_evVerified,
    HandlerFsm_evStart,
    HandlerFsm_evStop,
    HandlerFsm_evWrite,
    HandlerFsm_evStopped,
    HANDLERFSM_NO_MSG
};


};};};}; //namespace Cpl { namespace Rte { namespace Persistence { namespace Record 


#endif
