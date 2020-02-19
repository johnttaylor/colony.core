/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -doxygen -o HandlerFsm -l cppx -Trace HandlerFsm.cdd   */
/* This file is generated from HandlerFsm.cdd - do not edit manually  */
/* Generated on:  version 3.7.4b3 */


#ifndef __HANDLERFSM_CPL_DM_PERSISTENCE_RECORD_EXT_H__
#define __HANDLERFSM_CPL_DM_PERSISTENCE_RECORD_EXT_H__

// Events which can be sent to the state-machine

#ifndef DOXYGEN_WILL_SKIP_THIS

namespace Cpl { namespace Dm { namespace Persistence { namespace Record  {

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




#define Cpl_Dm_Persistence_Record_HandlerFsm_InnermostStates "Verifying,NoPersistence,Reading,Idle,Writeable,Stopping,ClearingFile,WaitingToOpen,Writing;";

}}}} //namespace Cpl { namespace Dm { namespace Persistence { namespace Record 


#endif // !DOXYGEN_WILL_SKIP_THIS

#endif
