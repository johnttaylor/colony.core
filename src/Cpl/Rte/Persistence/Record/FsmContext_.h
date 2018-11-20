#ifndef Cpl_Rte_Persistence_Record_FsmContext_x_h_
#define Cpl_Rte_Persistence_Record_FsmContext_x_h_
/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


/// Namespace(s)
namespace Cpl { namespace Rte { namespace Persistence { namespace Record { 


/// Context (aka actions/guards) for my Finite State Machine
class FsmContext_
{
public:
    /// Action
    virtual void connectToModel() throw() = 0;

    /// Action
    virtual void defaultData() throw() = 0;

    /// Action
    virtual void disconnectFromModel() throw() = 0;

    /// Action
    virtual void issueWrite() throw() = 0;

    /// Action
    virtual void markClean() throw() = 0;

    /// Action
    virtual void markDirty() throw() = 0;

    /// Action
    virtual void startTimer() throw() = 0;

    /// Action
    virtual void stopTimer() throw() = 0;

    /// Action
    virtual void tellInitialized() throw() = 0;

    /// Action
    virtual void tellStartCompleted() throw() = 0;

    /// Action
    virtual void tellStarting() throw() = 0;

    /// Action
    virtual void tellStopped() throw() = 0;


public:
    /// Guard
    virtual bool isDirty() throw() = 0;

    /// Guard
    virtual bool isLoadGood() throw() = 0;


public:
    /// Virtual Destructor
    virtual ~FsmContext_(){}

};

};};};};  /// end namespace(s)
#endif /// end header latch
