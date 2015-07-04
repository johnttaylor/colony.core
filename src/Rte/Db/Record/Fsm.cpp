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

/* Command line options: -p CADIFRA -doxygen -o Fsm -l cppx Fsm.cdd   */
/* This file is generated from Fsm.cdd - do not edit manually  */
/* Generated on:  version 3.6.9 */


/* Include default headers (no header definition in UML state-chart provided) */
/* Define a header comment in your statediagram.model to define own includes here. */
#include "Fsm_ext.h"
#include "Fsm.h"

namespace Rte { namespace Db { namespace Record  {

    Fsm::Fsm(void)
    {
        m_initialized=0U;
    }



    /* State names */
    const char states[]=
        "Verifying\0NoPersistence\0Reading \0Idle\0Opening\0Active\0Writeable\0Stopping\0ClearingDb\0WaitingToOpen\0Writing\0";

    const unsigned short state_idx[]={
        0,10,24,33,38,46,53,63,72,83,97,105};

    /* Event names */
    const char events[]=
        "evWrite\0evStop\0evVerified\0evResponse\0evDefault\0dbResponse\0evStart\0NO_MSG\0";

    const unsigned short evt_idx[]={
        0,8,15,26,37,47,58,66};

    const char* Fsm::getNameByState(unsigned short state) const {
        return states+state_idx[state];
    }

    const char* Fsm::getNameByEvent(FSM_EVENT_T evt) const {
        return events+evt_idx[evt];
    }


    // Helper(s) to reset history
    void Fsm::resetHistoryActive(void){
        stateVars.stateVarActive=NoPersistence;
    }

    void Fsm::resetHistoryOpening(void){
        stateVars.stateVarOpening=WaitingToOpen;
    }


    // Helper(s) to find out if the machine is in a certain state

    bool Fsm::isInVerifying(void) const {return(((stateVars.stateVarOpening== Verifying)&&(stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInNoPersistence(void) const {return(((stateVars.stateVarActive== NoPersistence)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInReading (void) const {return(((stateVars.stateVarOpening== Reading )&&(stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInIdle(void) const {return(((stateVars.stateVar== Idle)) ? (true) : (false));}
    bool Fsm::isInOpening(void) const {return(((stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInActive(void) const {return(((stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInWriteable(void) const {return(((stateVars.stateVarActive== Writeable)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInStopping(void) const {return(((stateVars.stateVar== Stopping)) ? (true) : (false));}
    bool Fsm::isInClearingDb(void) const {return(((stateVars.stateVarOpening== ClearingDb)&&(stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInWaitingToOpen(void) const {return(((stateVars.stateVarOpening== WaitingToOpen)&&(stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInWriting(void) const {return(((stateVars.stateVarActive== Writing)&&(stateVars.stateVar== Active)) ? (true) : (false));}

    // Helper to get id of innermost active state

    unsigned short Fsm::getInnermostActiveState(void) const {
        if(isInWaitingToOpen()){
            return WaitingToOpen;
        }else if(isInClearingDb()){
            return ClearingDb;
        }else if(isInReading ()){
            return Reading ;
        }else if(isInVerifying()){
            return Verifying;
        }else if(isInWriting()){
            return Writing;
        }else if(isInWriteable()){
            return Writeable;
        }else if(isInNoPersistence()){
            return NoPersistence;
        }else if(isInStopping()){
            return Stopping;
        }else if(isInIdle()){
            return Idle;
        }else{
            return NUM_STATES;
        }
    }
    // Initialize method. Must be called once to init the machine
    void Fsm::initialize(){
        //call on entry code of default states
        if(m_initialized==0U){
            m_initialized=1U;

            //Create copy of statevar
            stateVarsCopy = stateVars;

            // Set state vars to default states

            stateVarsCopy.stateVar = Idle; /* set init state of top state */
            stateVarsCopy.stateVarActive = NoPersistence; /* set init state of Active */
            stateVarsCopy.stateVarOpening = WaitingToOpen; /* set init state of Opening */

        // Copy state variables back
        stateVars = stateVarsCopy;

        }



    }

    // State machine event handler
    int Fsm::processEvent(FSM_EVENT_T msg){

        int evConsumed = 0U;

    

        if(m_initialized==0U) return 0;

        //Create copy of statevar
        stateVarsCopy = stateVars;


        switch (stateVars.stateVar) {

            case Idle:
                if(msg==evStart){
                    /* Transition from Idle to Opening */
                    evConsumed=1;

                    /* Action code for transition  */
                    requestDbOpen();


                    stateVarsCopy.stateVarActive = Opening;/* entry chain  */

                    /* adjust state variables  */
                    stateVarsCopy.stateVar = Active;
                }else if(msg==evWrite){
                    /* Transition from Idle to Idle */
                    evConsumed=1;

                    /* Action code for transition  */
                    queWriteRequest();


                    /* adjust state variables  */
                    stateVarsCopy.stateVar = Idle;
                }else{
                    /* Intentionally left blank */
                } /*end of event selection */
            break; /* end of case Idle  */

            case Active:

                switch (stateVars.stateVarActive) {

                    case Writeable:
                        if(msg==evWrite){
                            /* Transition from Writeable to Writing */
                            evConsumed=1;

                            /* Action code for transition  */
                            requestDbWrite();


                            /* adjust state variables  */
                            stateVarsCopy.stateVarActive = Writing;
                        }else{
                            /* Intentionally left blank */
                        } /*end of event selection */
                    break; /* end of case Writeable  */

                    case NoPersistence:
                        if(msg==evWrite){
                            /* Transition from NoPersistence to NoPersistence */
                            evConsumed=1;

                            /* Action code for transition  */
                            consumeNoWrite();
                            ackWrite();
                            inspectWriteQue();


                            /* adjust state variables  */
                            stateVarsCopy.stateVarActive = NoPersistence;
                        }else{
                            /* Intentionally left blank */
                        } /*end of event selection */
                    break; /* end of case NoPersistence  */

                    case Opening:

                        switch (stateVars.stateVarOpening) {

                            case WaitingToOpen:
                                if(msg==evResponse){
                                    if(isDbEof()){
                                        /* Transition from WaitingToOpen to Writeable */
                                        evConsumed=1;


                                        /* Action code for transition  */
                                        ackOpenDone();
                                        inspectWriteQue();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarActive = Writeable;
                                    }else if(isDbSuccess()){
                                        /* Transition from WaitingToOpen to Reading  */
                                        evConsumed=1;

                                        /* Action code for transition  */
                                        requestDbRead();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarOpening = Reading ;
                                    }else{
                                        /* Intentionally left blank */
                                    } /*end of event selection */
                                }else{
                                    /* Intentionally left blank */
                                } /*end of event selection */
                            break; /* end of case WaitingToOpen  */

                            case Reading :
                                if(msg==dbResponse){
                                    if(isDbBadData()){
                                        /* Transition from Reading  to ClearingDb */
                                        evConsumed=1;

                                        /* Action code for transition  */
                                        reportError();
                                        nakOpenDone();
                                        requestDbClear();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarOpening = ClearingDb;
                                    }else{
                                        /* Intentionally left blank */
                                    } /*end of event selection */
                                }else if(msg==evResponse){
                                    if(isDbEof()){
                                        /* Transition from Reading  to Verifying */
                                        evConsumed=1;

                                        /* Action code for transition  */
                                        verifyOpen();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarOpening = Verifying;
                                    }else if(isDbSuccess()){
                                        /* Transition from Reading  to Reading  */
                                        evConsumed=1;

                                        /* Action code for transition  */
                                        ackRead();
                                        requestDbRead();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarOpening = Reading ;
                                    }else{
                                        /* Intentionally left blank */
                                    } /*end of event selection */
                                }else{
                                    /* Intentionally left blank */
                                } /*end of event selection */
                            break; /* end of case Reading   */

                            case ClearingDb:
                                if(msg==evResponse){
                                    if(isDbSuccess()){
                                        /* Transition from ClearingDb to Writeable */
                                        evConsumed=1;


                                        /* Action code for transition  */
                                        inspectWriteQue();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarActive = Writeable;
                                    }else{
                                        /* Intentionally left blank */
                                    } /*end of event selection */
                                }else{
                                    /* Intentionally left blank */
                                } /*end of event selection */
                            break; /* end of case ClearingDb  */

                            case Verifying:
                                if(msg==evDefault){
                                    /* Transition from Verifying to ClearingDb */
                                    evConsumed=1;

                                    /* Action code for transition  */
                                    nakOpenDone();
                                    requestDbClear();


                                    /* adjust state variables  */
                                    stateVarsCopy.stateVarOpening = ClearingDb;
                                }else if(msg==evVerified){
                                    /* Transition from Verifying to Writeable */
                                    evConsumed=1;


                                    /* Action code for transition  */
                                    ackOpenDone();
                                    inspectWriteQue();


                                    /* adjust state variables  */
                                    stateVarsCopy.stateVarActive = Writeable;
                                }else{
                                    /* Intentionally left blank */
                                } /*end of event selection */
                            break; /* end of case Verifying  */

                            default:
                                /* Intentionally left blank */
                            break;
                        } /* end switch Opening */

                        /* Check if event was already processed  */
                        if(evConsumed==0){

                            if(msg==evResponse){
                                if(isDbError()){
                                    /* Transition from Opening to Active */
                                    evConsumed=1;
                                

                                    /* Action code for transition  */
                                    reportError();
                                    nakOpenDone();
                                    inspectWriteQue();

                                    stateVarsCopy.stateVar = Active;/* Default in entry chain  */
                                    stateVarsCopy.stateVarActive = NoPersistence;/* Default in entry chain  */

                                }else{
                                    /* Intentionally left blank */
                                } /*end of event selection */
                            }else if(msg==evWrite){
                                /* Transition from Opening to Opening */
                                evConsumed=1;
                                
                                /* Action code for transition  */
                                queWriteRequest();

                                stateVarsCopy.stateVarActive = Opening;/* entry chain  */

                            }else{
                                /* Intentionally left blank */
                            } /*end of event selection */
                        }
                    break; /* end of case Opening  */

                    case Writing:
                        if(msg==evResponse){
                            if(isDbSuccess()){
                                /* Transition from Writing to Writeable */
                                evConsumed=1;

                                /* Action code for transition  */
                                
                                ackWrite();
                                inspectWriteQue();


                                /* adjust state variables  */
                                stateVarsCopy.stateVarActive = Writeable;
                            }else{
                                /* Transition from Writing to Active */
                                evConsumed=1;


                                /* Action code for transition  */
                                
                                reportError();
                                inspectWriteQue();

                                stateVarsCopy.stateVar = Active;/* Default in entry chain  */
                                stateVarsCopy.stateVarActive = NoPersistence;/* Default in entry chain  */

                            } /*end of event selection */
                        }else if(msg==evWrite){
                            /* Transition from Writing to Writing */
                            evConsumed=1;

                            /* Action code for transition  */
                            queWriteRequest();


                            /* adjust state variables  */
                            stateVarsCopy.stateVarActive = Writing;
                        }else{
                            /* Intentionally left blank */
                        } /*end of event selection */
                    break; /* end of case Writing  */

                    default:
                        /* Intentionally left blank */
                    break;
                } /* end switch Active */

                /* Check if event was already processed  */
                if(evConsumed==0){

                    if(msg==evStop){
                        /* Transition from Active to Stopping */
                        evConsumed=1;
                        
                        if(stateVars.stateVarActive== Opening){
                            
                        }

                        /* Action code for transition  */
                        requestDbClose();
                        clearWriteQue();


                        /* adjust state variables  */
                        stateVarsCopy.stateVar = Stopping;
                    }else{
                        /* Intentionally left blank */
                    } /*end of event selection */
                }
            break; /* end of case Active  */

            case Stopping:
                if(msg==evResponse){
                    /* Transition from Stopping to Idle */
                    evConsumed=1;

                    /* Action code for transition  */
                    ackDbStopped();


                    /* adjust state variables  */
                    stateVarsCopy.stateVar = Idle;
                }else{
                    /* Intentionally left blank */
                } /*end of event selection */
            break; /* end of case Stopping  */

            default:
                /* Intentionally left blank */
            break;
        } /* end switch stateVar_root */
    // Copy state variables back
    stateVars = stateVarsCopy;

        return evConsumed;
    } // end processEvent







};};}; //namespace Rte { namespace Db { namespace Record 

