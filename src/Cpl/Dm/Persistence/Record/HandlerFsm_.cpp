/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -doxygen -o HandlerFsm -l cppx -Trace HandlerFsm.cdd   */
/* This file is generated from HandlerFsm.cdd - do not edit manually  */
/* Generated on:  version 3.7 */


/* Include default headers (no header definition in UML state-chart provided) */
/* Define a header comment in your statediagram.model to define own includes here. */
#include "Cpl/Dm/Persistence/Record/HandlerFsm_ext_.h"
#include "Cpl/Dm/Persistence/Record/HandlerFsm_.h"
#include "Cpl/Dm/Persistence/Record/HandlerFsm_trace_.h"

namespace Cpl { namespace Dm { namespace Persistence { namespace Record  {

    HandlerFsm::HandlerFsm(void)
    {
        m_initialized=0U;
    }



    /* State names */
    const char states[]=
        "Verifying\0NoPersistence\0Reading\0Idle\0Opening\0Active\0Writeable\0Stopping\0ClearingFile\0WaitingToOpen\0Writing\0";

    const unsigned short state_idx[]={
        0,10,24,32,37,45,52,62,71,84,98,106};

    /* Event names */
    const char events[]=
        "HandlerFsm_evIncompleteLoad\0HandlerFsm_evResponse\0HandlerFsm_evVerified\0HandlerFsm_evStart\0HandlerFsm_evStop\0HandlerFsm_evWrite\0HandlerFsm_evStopped\0NO_MSG\0";

    const unsigned short evt_idx[]={
        0,28,50,72,91,109,128,149};

    const char* HandlerFsm::getNameByState(unsigned short state) const {
        return states+state_idx[state];
    }

    const char* HandlerFsm::getNameByEvent(HANDLERFSM_EVENT_T evt) const {
        return events+evt_idx[evt];
    }


    // Helper(s) to reset history
    void HandlerFsm::resetHistoryActive(void){
        stateVars.stateVarActive=NoPersistence;
    }

    void HandlerFsm::resetHistoryOpening(void){
        stateVars.stateVarOpening=WaitingToOpen;
    }


    // Helper(s) to find out if the machine is in a certain state

    bool HandlerFsm::isInVerifying(void) const {return(((stateVars.stateVarOpening== Verifying)&&(stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool HandlerFsm::isInNoPersistence(void) const {return(((stateVars.stateVarActive== NoPersistence)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool HandlerFsm::isInReading(void) const {return(((stateVars.stateVarOpening== Reading)&&(stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool HandlerFsm::isInIdle(void) const {return(((stateVars.stateVar== Idle)) ? (true) : (false));}
    bool HandlerFsm::isInOpening(void) const {return(((stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool HandlerFsm::isInActive(void) const {return(((stateVars.stateVar== Active)) ? (true) : (false));}
    bool HandlerFsm::isInWriteable(void) const {return(((stateVars.stateVarActive== Writeable)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool HandlerFsm::isInStopping(void) const {return(((stateVars.stateVar== Stopping)) ? (true) : (false));}
    bool HandlerFsm::isInClearingFile(void) const {return(((stateVars.stateVarOpening== ClearingFile)&&(stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool HandlerFsm::isInWaitingToOpen(void) const {return(((stateVars.stateVarOpening== WaitingToOpen)&&(stateVars.stateVarActive== Opening)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool HandlerFsm::isInWriting(void) const {return(((stateVars.stateVarActive== Writing)&&(stateVars.stateVar== Active)) ? (true) : (false));}

    // Helper to get id of innermost active state

    unsigned short HandlerFsm::getInnermostActiveState(void) const {
        if(isInWaitingToOpen()){
            return WaitingToOpen;
        }else if(isInClearingFile()){
            return ClearingFile;
        }else if(isInReading()){
            return Reading;
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
    void HandlerFsm::initialize(){
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
    int HandlerFsm::processEvent(HANDLERFSM_EVENT_T msg){

        int evConsumed = 0U;

    

        if(m_initialized==0U) return 0;

        //Create copy of statevar
        stateVarsCopy = stateVars;


        switch (stateVars.stateVar) {

            case Idle:
                if(msg==HandlerFsm_evStart){
                    /* Transition from Idle to Opening */
                    evConsumed=1;

                    /* Action code for transition  */
                    requestFileOpen();


                    stateVarsCopy.stateVarActive = Opening;/* entry chain  */

                    /* adjust state variables  */
                    stateVarsCopy.stateVar = Active;
                    HandlerFsmTraceEvent(0);
                }else if(msg==HandlerFsm_evWrite){
                    /* Transition from Idle to Idle */
                    evConsumed=1;

                    /* Action code for transition  */
                    queWriteRequest();


                    /* adjust state variables  */
                    stateVarsCopy.stateVar = Idle;
                    HandlerFsmTraceEvent(2);
                }else{
                    /* Intentionally left blank */
                } /*end of event selection */
            break; /* end of case Idle  */

            case Active:

                switch (stateVars.stateVarActive) {

                    case Writeable:
                        if(msg==HandlerFsm_evWrite){
                            /* Transition from Writeable to Writing */
                            evConsumed=1;

                            /* Action code for transition  */
                            requestFileWrite();


                            /* adjust state variables  */
                            stateVarsCopy.stateVarActive = Writing;
                            HandlerFsmTraceEvent(2);
                        }else{
                            /* Intentionally left blank */
                        } /*end of event selection */
                    break; /* end of case Writeable  */

                    case NoPersistence:
                        if(msg==HandlerFsm_evWrite){
                            /* Transition from NoPersistence to NoPersistence */
                            evConsumed=1;

                            /* Action code for transition  */
                            consumeNoWrite();
                            ackWrite();
                            inspectWriteQue();


                            /* adjust state variables  */
                            stateVarsCopy.stateVarActive = NoPersistence;
                            HandlerFsmTraceEvent(2);
                        }else{
                            /* Intentionally left blank */
                        } /*end of event selection */
                    break; /* end of case NoPersistence  */

                    case Opening:

                        switch (stateVars.stateVarOpening) {

                            case WaitingToOpen:
                                if(msg==HandlerFsm_evResponse){
                                    if(isFileEof()){
                                        /* Transition from WaitingToOpen to Writeable */
                                        evConsumed=1;


                                        /* Action code for transition  */
                                        ackOpenDone();
                                        inspectWriteQue();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarActive = Writeable;
                                        HandlerFsmTraceEvent(10);
                                    }else if(isFileSuccess()){
                                        /* Transition from WaitingToOpen to Reading */
                                        evConsumed=1;

                                        /* Action code for transition  */
                                        requestFileRead();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarOpening = Reading;
                                        HandlerFsmTraceEvent(8);
                                    }else{
                                        /* Intentionally left blank */
                                    } /*end of event selection */
                                }else{
                                    /* Intentionally left blank */
                                } /*end of event selection */
                            break; /* end of case WaitingToOpen  */

                            case Reading:
                                if(msg==HandlerFsm_evResponse){
                                    if(isFileEof()){
                                        /* Transition from Reading to Verifying */
                                        evConsumed=1;

                                        /* Action code for transition  */
                                        verifyOpen();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarOpening = Verifying;
                                        HandlerFsmTraceEvent(10);
                                    }else if(isFileBadData()){
                                        /* Transition from Reading to ClearingFile */
                                        evConsumed=1;

                                        /* Action code for transition  */
                                        reportDataCorruptError();
                                        nakOpenDone();
                                        requestFileClear();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarOpening = ClearingFile;
                                        HandlerFsmTraceEvent(9);
                                    }else if(isFileSuccess()){
                                        /* Transition from Reading to Reading */
                                        evConsumed=1;

                                        /* Action code for transition  */
                                        ackRead();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarOpening = Reading;
                                        HandlerFsmTraceEvent(8);
                                    }else{
                                        /* Intentionally left blank */
                                    } /*end of event selection */
                                }else{
                                    /* Intentionally left blank */
                                } /*end of event selection */
                            break; /* end of case Reading  */

                            case ClearingFile:
                                if(msg==HandlerFsm_evResponse){
                                    if(!isFileError()){
                                        /* Transition from ClearingFile to Writeable */
                                        evConsumed=1;


                                        /* Action code for transition  */
                                        inspectWriteQue();


                                        /* adjust state variables  */
                                        stateVarsCopy.stateVarActive = Writeable;
                                        HandlerFsmTraceEvent(12);
                                    }else{
                                        /* Intentionally left blank */
                                    } /*end of event selection */
                                }else{
                                    /* Intentionally left blank */
                                } /*end of event selection */
                            break; /* end of case ClearingFile  */

                            case Verifying:
                                if(msg==HandlerFsm_evIncompleteLoad){
                                    /* Transition from Verifying to ClearingFile */
                                    evConsumed=1;

                                    /* Action code for transition  */
                                    reportMinorUpgrade();
                                    nakOpenDone();
                                    requestFileClear();


                                    /* adjust state variables  */
                                    stateVarsCopy.stateVarOpening = ClearingFile;
                                    HandlerFsmTraceEvent(6);
                                }else if(msg==HandlerFsm_evVerified){
                                    /* Transition from Verifying to Writeable */
                                    evConsumed=1;


                                    /* Action code for transition  */
                                    ackOpenDone();
                                    inspectWriteQue();


                                    /* adjust state variables  */
                                    stateVarsCopy.stateVarActive = Writeable;
                                    HandlerFsmTraceEvent(5);
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

                            if(msg==HandlerFsm_evResponse){
                                if(isNotCompatible()){
                                    /* Transition from Opening to Active */
                                    evConsumed=1;
                                

                                    /* Action code for transition  */
                                    reportIncompatible();
                                    defaultAllRecords();
                                    nakOpenDone();
                                    inspectWriteQue();

                                    stateVarsCopy.stateVar = Active;/* Default in entry chain  */
                                    stateVarsCopy.stateVarActive = NoPersistence;/* Default in entry chain  */

                                    HandlerFsmTraceEvent(13);
                                }else if(isFileError()){
                                    /* Transition from Opening to Active */
                                    evConsumed=1;
                                

                                    /* Action code for transition  */
                                    reportFileReadError();
                                    nakOpenDone();
                                    inspectWriteQue();

                                    stateVarsCopy.stateVar = Active;/* Default in entry chain  */
                                    stateVarsCopy.stateVarActive = NoPersistence;/* Default in entry chain  */

                                    HandlerFsmTraceEvent(7);
                                }else{
                                    /* Intentionally left blank */
                                } /*end of event selection */
                            }else if(msg==HandlerFsm_evWrite){
                                /* Transition from Opening to Opening */
                                evConsumed=1;
                                
                                /* Action code for transition  */
                                queWriteRequest();

                                stateVarsCopy.stateVarActive = Opening;/* entry chain  */

                                HandlerFsmTraceEvent(2);
                            }else{
                                /* Intentionally left blank */
                            } /*end of event selection */
                        }
                    break; /* end of case Opening  */

                    case Writing:
                        if(msg==HandlerFsm_evResponse){
                            if(isFileSuccess()){
                                /* Transition from Writing to Writeable */
                                evConsumed=1;

                                /* Action code for transition  */
                                ackWrite();
                                inspectWriteQue();


                                /* adjust state variables  */
                                stateVarsCopy.stateVarActive = Writeable;
                                HandlerFsmTraceEvent(8);
                            }else if(!isFileSuccess()){
                                /* Transition from Writing to Active */
                                evConsumed=1;


                                /* Action code for transition  */
                                nakWrite();
                                reportFileWriteError();
                                inspectWriteQue();

                                stateVarsCopy.stateVar = Active;/* Default in entry chain  */
                                stateVarsCopy.stateVarActive = NoPersistence;/* Default in entry chain  */

                                HandlerFsmTraceEvent(11);
                            }else{
                                /* Intentionally left blank */
                            } /*end of event selection */
                        }else if(msg==HandlerFsm_evWrite){
                            /* Transition from Writing to Writing */
                            evConsumed=1;

                            /* Action code for transition  */
                            queWriteRequest();


                            /* adjust state variables  */
                            stateVarsCopy.stateVarActive = Writing;
                            HandlerFsmTraceEvent(2);
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

                    if(msg==HandlerFsm_evStop){
                        /* Transition from Active to Stopping */
                        evConsumed=1;
                        
                        if(stateVars.stateVarActive== Opening){
                            
                        }

                        /* Action code for transition  */
                        requestFileClose();


                        /* adjust state variables  */
                        stateVarsCopy.stateVar = Stopping;
                        HandlerFsmTraceEvent(1);
                    }else{
                        /* Intentionally left blank */
                    } /*end of event selection */
                }
            break; /* end of case Active  */

            case Stopping:
                if(msg==HandlerFsm_evStopped){
                    /* Transition from Stopping to Idle */
                    evConsumed=1;

                    /* Action code for transition  */
                    ackFileStopped();
                    clearWriteQue();


                    /* adjust state variables  */
                    stateVarsCopy.stateVar = Idle;
                    HandlerFsmTraceEvent(3);
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
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "New State=%s", getNameByState(getInnermostActiveState()) ));

        return evConsumed;
    } // end processEvent







};};};}; //namespace Cpl { namespace Dm { namespace Persistence { namespace Record 

