#ifndef Cpl_Rte_Persistence_Record_HandlerFsmContext_x_h_
#define Cpl_Rte_Persistence_Record_HandlerFsmContext_x_h_
/*-----------------------------------------------------------------------------
* Blah..
*----------------------------------------------------------------------------*/
/** @file */



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


/// Namespace(s)
namespace Cpl { namespace Rte { namespace Persistence { namespace Record { 


/// Context (aka actions/guards) for my Finite State Machine
class HandlerFsmContext_
{
public:
    /// Action
    virtual void ackFileStopped() throw() = 0;

    /// Action
    virtual void ackOpenDone() throw() = 0;

    /// Action
    virtual void ackRead() throw() = 0;

    /// Action
    virtual void ackWrite() throw() = 0;

    /// Action
    virtual void clearWriteQue() throw() = 0;

    /// Action
    virtual void consumeNoWrite() throw() = 0;

    /// Action
    virtual void defaultAllRecords() throw() = 0;

    /// Action
    virtual void inspectWriteQue() throw() = 0;

    /// Action
    virtual void nakOpenDone() throw() = 0;

    /// Action
    virtual void nakWrite() throw() = 0;

    /// Action
    virtual void queWriteRequest() throw() = 0;

    /// Action
    virtual void reportDataCorruptError() throw() = 0;

    /// Action
    virtual void reportFileReadError() throw() = 0;

    /// Action
    virtual void reportFileWriteError() throw() = 0;

    /// Action
    virtual void reportIncompatible() throw() = 0;

    /// Action
    virtual void reportMinorUpgrade() throw() = 0;

    /// Action
    virtual void requestFileClear() throw() = 0;

    /// Action
    virtual void requestFileClose() throw() = 0;

    /// Action
    virtual void requestFileOpen() throw() = 0;

    /// Action
    virtual void requestFileRead() throw() = 0;

    /// Action
    virtual void requestFileWrite() throw() = 0;

    /// Action
    virtual void verifyOpen() throw() = 0;


public:
    /// Guard
    virtual bool isFileBadData() throw() = 0;

    /// Guard
    virtual bool isFileEof() throw() = 0;

    /// Guard
    virtual bool isFileError() throw() = 0;

    /// Guard
    virtual bool isFileSuccess() throw() = 0;

    /// Guard
    virtual bool isNotCompatible() throw() = 0;


public:
    /// Virtual Destructor
    virtual ~HandlerFsmContext_(){}

};

};};};};  /// end namespace(s)
#endif /// end header latch
