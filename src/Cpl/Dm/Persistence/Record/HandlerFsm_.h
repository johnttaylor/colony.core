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
/* This file is generated from HandlerFsm.cdd - do not edit manually  */
/* Generated on:  version 3.7.4b3 */


#ifndef __HANDLERFSM_CPL_DM_PERSISTENCE_RECORD_H__
#define __HANDLERFSM_CPL_DM_PERSISTENCE_RECORD_H__

/** \class Cpl::Dm::Persistence::Record::HandlerFsm

Here is the graph that shows the state machine this class implements

\dot
digraph G {
rankdir=TB;
bgcolor=transparent;
node[fontsize=8 shape=box];
edge[fontsize=8, arrowsize=0.5];
compound=true;
remincross=true;
nodesep=0.3;
ranksep=.4;
"Idle"->"defaultOpening"[labeldistance = 2.0, taillabel=<evStart / <br ALIGN="LEFT"/><br ALIGN="LEFT"/>requestFileOpen();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black lhead=clusterOpening];//Idle Opening
"defaultActive"->"Stopping"[labeldistance=2.0, headlabel=<evStop / <br ALIGN="LEFT"/><br ALIGN="LEFT"/>requestFileClose();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black ltail=clusterActive];//Active Stopping
"NoPersistence"->"NoPersistence"[label=<evWrite / <br ALIGN="LEFT"/>consumeNoWrite();<br ALIGN="LEFT"/>ackWrite();<br ALIGN="LEFT"/>in...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //NoPersistence NoPersistence
"Stopping"->"Idle"[label=<evStopped / <br ALIGN="LEFT"/>ackFileStopped();<br ALIGN="LEFT"/>clearWriteQue(...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Stopping Idle
"defaultOpening"->"defaultActive"[label=<evResponse<br ALIGN="LEFT"/>[isFileError()] / <br ALIGN="LEFT"/>reportFileReadError();<br ALIGN="LEFT"/>nakOpenDo...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black];//Opening Active
"WaitingToOpen"->"Reading"[label=<evResponse<br ALIGN="LEFT"/>[isFileSuccess()] / requestFileRead();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //WaitingToOpen Reading
"defaultroot"->"Idle"[label=<  > fontname=Arial, style=dotted];
"Reading"->"Reading"[label=<evResponse<br ALIGN="LEFT"/>[isFileSuccess()] / ackRead();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Reading Reading
"Reading"->"ClearingFile"[label=<evResponse<br ALIGN="LEFT"/>[isFileBadData()] / reportDataCorruptError();<br ALIGN="LEFT"/>nakOpe...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Reading ClearingFile
"Reading"->"Verifying"[label=<evResponse<br ALIGN="LEFT"/>[isFileEof()] / verifyOpen();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Reading Verifying
"defaultActive"->"NoPersistence"[label=<  > fontname=Arial, style=dotted];
"Writing"->"defaultActive"[labeldistance = 2.0, taillabel=<evResponse<br ALIGN="LEFT"/>[!isFileSuccess()] / <br ALIGN="LEFT"/>nakWrite();<br ALIGN="LEFT"/>reportFileWriteError...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black lhead=clusterActive];//Writing Active
"ClearingFile"->"Writeable"[label=<evResponse<br ALIGN="LEFT"/>[!isFileError()] / inspectWriteQue();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //ClearingFile Writeable
"WaitingToOpen"->"Writeable"[label=<evResponse<br ALIGN="LEFT"/>[isFileEof()] / ackOpenDone();<br ALIGN="LEFT"/>inspectWriteQue()...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //WaitingToOpen Writeable
"Writeable"->"Writing"[label=<evWrite / <br ALIGN="LEFT"/>requestFileWrite();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Writeable Writing
"Writing"->"Writeable"[label=<evResponse<br ALIGN="LEFT"/>[isFileSuccess()] / ackWrite();<br ALIGN="LEFT"/>inspectWriteQue();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Writing Writeable
"Verifying"->"Writeable"[label=<evVerified / <br ALIGN="LEFT"/>ackOpenDone();<br ALIGN="LEFT"/>inspectWriteQue()...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Verifying Writeable
"defaultOpening"->"defaultOpening"[label=<evWrite / <br ALIGN="LEFT"/><br ALIGN="LEFT"/>queWriteRequest();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black];//Opening Opening
"Idle"->"Idle"[label=<evWrite / <br ALIGN="LEFT"/>queWriteRequest();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Idle Idle
"Writing"->"Writing"[label=<evWrite / <br ALIGN="LEFT"/>queWriteRequest();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Writing Writing
"defaultOpening"->"WaitingToOpen"[label=<  > fontname=Arial, style=dotted];
"Verifying"->"ClearingFile"[label=<evIncompleteLoad / <br ALIGN="LEFT"/>reportMinorUpgrade();<br ALIGN="LEFT"/>nakOpenDon...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Verifying ClearingFile
"defaultOpening"->"defaultActive"[label=<evResponse<br ALIGN="LEFT"/>[isNotCompatible()] / <br ALIGN="LEFT"/>reportIncompatible();<br ALIGN="LEFT"/>defaultAll...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black];//Opening Active
"Idle"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Idle</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];subgraph "clusterActive"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Active<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"Writeable"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Writeable</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"NoPersistence"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>NoPersistence</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];subgraph "clusterOpening"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Opening(H)<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"WaitingToOpen"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>WaitingToOpen</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Reading"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Reading</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"ClearingFile"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>ClearingFile</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Verifying"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Verifying</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"defaultOpening"[label=<(H)>,shape=circle, fontsize=8, height=0.2, width=0.2,fixedsize=true, fontname=Arial, fontcolor=white,fillcolor=black, style=filled];
};
"defaultActive"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
"Writing"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Writing</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];};
"Stopping"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Stopping</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"defaultroot"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
}
\enddot
*/

#include "Cpl/Dm/Persistence/Record/HandlerFsm_ext_.h"
#include "Cpl/Dm/Persistence/Record/HandlerFsmContext_.h"

#ifndef DOXYGEN_WILL_SKIP_THIS

namespace Cpl { namespace Dm { namespace Persistence { namespace Record  {

    class HandlerFsm: public HandlerFsmContext_
    {
        public:
            HandlerFsm(void);

            int processEvent(HANDLERFSM_EVENT_T msg);

            void initialize();

            // Helper(s) to reset history
            void resetHistoryActive(void);
            void resetHistoryOpening(void);

            // Helper(s) to find out if the machine is in a certain state
            bool isInVerifying(void) const;
            bool isInNoPersistence(void) const;
            bool isInReading(void) const;
            bool isInIdle(void) const;
            bool isInOpening(void) const;
            bool isInActive(void) const;
            bool isInWriteable(void) const;
            bool isInStopping(void) const;
            bool isInClearingFile(void) const;
            bool isInWaitingToOpen(void) const;
            bool isInWriting(void) const;


            // Helper returning the innermost active state id.
            unsigned short getInnermostActiveState(void) const;


            // Returns the state name as string
            const char* getNameByState(unsigned short state) const;

            // Returns the event as string
            const char* getNameByEvent(HANDLERFSM_EVENT_T evt) const;

            enum States{
                Verifying,
                NoPersistence,
                Reading,
                Idle,
                Opening,
                Active,
                Writeable,
                Stopping,
                ClearingFile,
                WaitingToOpen,
                Writing,
                NUM_STATES  // number of states in the machine
            };

        protected:
            int m_initialized;



        protected:
            // State variables
            struct stateVarsT{
                States stateVar;
                States stateVarActive;
                States stateVarOpening;
            };
            stateVarsT stateVars;
            stateVarsT stateVarsCopy;



            /* Region handler prototypes  */
    };

}}}} //namespace Cpl { namespace Dm { namespace Persistence { namespace Record 


#endif // !DOXYGEN_WILL_SKIP_THIS

#endif // __HANDLERFSM_CPL_DM_PERSISTENCE_RECORD_H__
