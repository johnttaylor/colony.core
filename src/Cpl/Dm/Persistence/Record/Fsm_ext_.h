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
/* This file is generated from Fsm.cdd - do not edit manually  */
/* Generated on:  version 3.7.4b2 */


#ifndef __FSM_CPL_DM_PERSISTENCE_RECORD_EXT_H__
#define __FSM_CPL_DM_PERSISTENCE_RECORD_EXT_H__

// Events which can be sent to the state-machine

#ifndef DOXYGEN_WILL_SKIP_THIS

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




#define InnermostStates "Clean,DelayingWrite,Writing,WaitingOnRead,Initialized,WriteDefaulting,WritingRecord,ReadDefaulting,Idle,Defaulting;"

}}}} //namespace Cpl { namespace Dm { namespace Persistence { namespace Record 


#endif // !DOXYGEN_WILL_SKIP_THIS

#endif
