#ifndef Rte_Db_Record_FsmContext_x_h_
#define Rte_Db_Record_FsmContext_x_h_
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



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


/// Namespace(s)
namespace Rte { namespace Db { namespace Record { 


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

};};};  /// end namespace(s)
#endif /// end header latch
