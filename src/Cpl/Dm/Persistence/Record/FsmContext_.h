#ifndef Cpl_Dm_Persistence_Record_FsmContext_x_h_
#define Cpl_Dm_Persistence_Record_FsmContext_x_h_
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



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


/// Namespace(s)
namespace Cpl { namespace Dm { namespace Persistence { namespace Record { 


/// Context (aka actions/guards) for my Finite State Machine
class FsmContext_
{
public:
    /// Action
    virtual void connectToModel() noexcept = 0;

    /// Action
    virtual void defaultData() noexcept = 0;

    /// Action
    virtual void disconnectFromModel() noexcept = 0;

    /// Action
    virtual void issueWrite() noexcept = 0;

    /// Action
    virtual void markClean() noexcept = 0;

    /// Action
    virtual void markDirty() noexcept = 0;

    /// Action
    virtual void startTimer() noexcept = 0;

    /// Action
    virtual void stopTimer() noexcept = 0;

    /// Action
    virtual void tellInitialized() noexcept = 0;

    /// Action
    virtual void tellStartCompleted() noexcept = 0;

    /// Action
    virtual void tellStarting() noexcept = 0;

    /// Action
    virtual void tellStopped() noexcept = 0;


public:
    /// Guard
    virtual bool isDirty() noexcept = 0;

    /// Guard
    virtual bool isLoadGood() noexcept = 0;


public:
    /// Virtual Destructor
    virtual ~FsmContext_(){}

};

};};};};  /// end namespace(s)
#endif /// end header latch
