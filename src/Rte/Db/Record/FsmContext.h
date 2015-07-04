#ifndef Rte_Db_Record_FsmContext_h_
#define Rte_Db_Record_FsmContext_h_
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
class FsmContext
{
public:
    /// Action
    virtual void ackWrite() throw() = 0;

    /// Action
    virtual void consumeNoWrite() throw() = 0;

    /// Action
    virtual void requestDbClose() throw() = 0;

    /// Action
    virtual void reportError() throw() = 0;

    /// Action
    virtual void queWriteRequest() throw() = 0;

    /// Action
    virtual void ackRead() throw() = 0;

    /// Action
    virtual void clearWriteQue() throw() = 0;

    /// Action
    virtual void nakOpenDone() throw() = 0;

    /// Action
    virtual void requestDbOpen() throw() = 0;

    /// Action
    virtual void ackDbStopped() throw() = 0;

    /// Action
    virtual void verifyOpen() throw() = 0;

    /// Action
    virtual void ackOpenDone() throw() = 0;

    /// Action
    virtual void requestDbClear() throw() = 0;

    /// Action
    virtual void requestDbWrite() throw() = 0;

    /// Action
    virtual void inspectWriteQue() throw() = 0;

    /// Action
    virtual void requestDbRead() throw() = 0;


public:
    /// Guard
    virtual bool isDbEof() throw() = 0;

    /// Guard
    virtual bool isDbSuccess() throw() = 0;

    /// Guard
    virtual bool isDbBadData() throw() = 0;

    /// Guard
    virtual bool isDbError() throw() = 0;


public:
    /// Virtual Destructor
    virtual ~FsmContext(){}

};

};};};  /// end namespace(s)
#endif /// end header latch
