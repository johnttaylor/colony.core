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

/* Command line options: -p CADIFRA -doxygen -o Fsm -l cppx -Trace Fsm.cdd   */
/* This file is generated from Fsm.cdd - do not edit manually  */
/* Generated on:  version 3.6.9 */


#ifndef __FSM_RTE_DB_RECORD_EXT_H__
#define __FSM_RTE_DB_RECORD_EXT_H__

// Events which can be sent to the state-machine

namespace Rte { namespace Db { namespace Record  {

enum FSM_EVENT_T {
    evWrite=0U,
    evStop,
    evVerified,
    evResponse,
    evDefault,
    evStopped,
    evStart,
    FSM_NO_MSG
};


};};}; //namespace Rte { namespace Db { namespace Record 


#endif
