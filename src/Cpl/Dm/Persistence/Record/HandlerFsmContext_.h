#ifndef Cpl_Dm_Persistence_Record_HandlerFsmContext_x_h_
#define Cpl_Dm_Persistence_Record_HandlerFsmContext_x_h_
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
class HandlerFsmContext_
{
public:
    /// Action
    virtual void ackFileStopped() noexcept = 0;

    /// Action
    virtual void ackOpenDone() noexcept = 0;

    /// Action
    virtual void ackRead() noexcept = 0;

    /// Action
    virtual void ackWrite() noexcept = 0;

    /// Action
    virtual void clearWriteQue() noexcept = 0;

    /// Action
    virtual void consumeNoWrite() noexcept = 0;

    /// Action
    virtual void defaultAllRecords() noexcept = 0;

    /// Action
    virtual void inspectWriteQue() noexcept = 0;

    /// Action
    virtual void nakOpenDone() noexcept = 0;

    /// Action
    virtual void nakWrite() noexcept = 0;

    /// Action
    virtual void queWriteRequest() noexcept = 0;

    /// Action
    virtual void reportDataCorruptError() noexcept = 0;

    /// Action
    virtual void reportFileReadError() noexcept = 0;

    /// Action
    virtual void reportFileWriteError() noexcept = 0;

    /// Action
    virtual void reportIncompatible() noexcept = 0;

    /// Action
    virtual void reportMinorUpgrade() noexcept = 0;

    /// Action
    virtual void requestFileClear() noexcept = 0;

    /// Action
    virtual void requestFileClose() noexcept = 0;

    /// Action
    virtual void requestFileOpen() noexcept = 0;

    /// Action
    virtual void requestFileRead() noexcept = 0;

    /// Action
    virtual void requestFileWrite() noexcept = 0;

    /// Action
    virtual void verifyOpen() noexcept = 0;


public:
    /// Guard
    virtual bool isFileBadData() noexcept = 0;

    /// Guard
    virtual bool isFileEof() noexcept = 0;

    /// Guard
    virtual bool isFileError() noexcept = 0;

    /// Guard
    virtual bool isFileSuccess() noexcept = 0;

    /// Guard
    virtual bool isNotCompatible() noexcept = 0;


public:
    /// Virtual Destructor
    virtual ~HandlerFsmContext_(){}

};

};};};};  /// end namespace(s)
#endif /// end header latch
