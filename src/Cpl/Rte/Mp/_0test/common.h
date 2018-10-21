#ifndef common_h_
#define common_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2018  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Cpl/Rte/MailboxServer.h"
#include "Cpl/Rte/SubscriberComposer.h"
#include "Cpl/Rte/RmwComposer.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Timer/Local.h"
#include "Cpl/Rte/Mp/Int32.h"
#include "Cpl/Rte/Mp/Uint64.h"
#include "Cpl/Rte/Mp/Int64.h"
#include "Cpl/Rte/Mp/Bool.h"
#include "Cpl/Rte/Mp/Float.h"
#include "Cpl/Rte/Mp/Double.h"
#include "Cpl/Rte/Mp/String.h"
#include "Cpl/Rte/Mp/RefCounter.h"
#include "Cpl/Rte/Mp/ArrayUint8.h"

/// 
using namespace Cpl::Rte;

#define SECT_   "_0test"


/////////////////////////////////////////////////////////////////
class ViewerBase : public Cpl::Itc::CloseSync
{
public:
    ///
    volatile bool                       m_opened;
    ///
    Cpl::System::Thread&                m_masterThread;
    ///
    Cpl::Itc::OpenRequest::OpenMsg*     m_pendingOpenMsgPtr;
    ///
    uint16_t                            m_lastSeqNumber;
    ///
    int8_t                              m_lastValidState;
    ///
    bool                                m_done;
    ///
    uint32_t                            m_notifCount;

    /// Constructor
    ViewerBase( MailboxServer& myMbox, Cpl::System::Thread& masterThread )
        :Cpl::Itc::CloseSync( myMbox )
        , m_opened( false )
        , m_masterThread( masterThread )
        , m_pendingOpenMsgPtr( 0 )
        , m_lastSeqNumber( ModelPoint::SEQUENCE_NUMBER_UNKNOWN )
        , m_lastValidState( OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID )
        , m_done( false )
        , m_notifCount( 0 )
    {
    }

public:
    ///
    virtual void subscribe() = 0;
    ///
    virtual void unsubscribe() = 0;
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        if ( m_opened )
        {
            FAIL( "OPENING ViewerBase more than ONCE" );
        }

        m_notifCount       = 0;
        m_done              = false;
        m_pendingOpenMsgPtr = &msg;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerBase::SUBSCRIBING (%p) for Change notification.", this) );

        // Subscribe to my model point
        subscribe();

        // Note: The open message will be returned once all the model point receives its initial callback
    }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        if ( !m_opened )
        {
            FAIL( "CLOSING ViewerBase more than ONCE" );
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerBase(%p): Closing... ", this) );

        // Un-subscribe to my model point
        unsubscribe();
        m_opened = false;
        msg.returnToSender();
    }
};

/////////////////////////////////////////////////////////////////
class ViewerInt32 : public ViewerBase, public Mp::Int32::Observer
{
public:
    ///
    Mp::Int32&  m_mp1;

    /// Constructor
    ViewerInt32( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::Int32& mp1 )
        :ViewerBase( myMbox, masterThread )
        , Mp::Int32::Observer( myMbox )
        , m_mp1( mp1 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerInt32(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Mp::Int32& modelPointThatChanged ) throw()
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerInt32(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerInt32(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= 2 )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

class RmwInt32 : public Mp::Int32::Client
{
public:
    ///
    int m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T m_returnResult;
    ///
    int32_t                         m_incValue;

public:
    ///
    RmwInt32():m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_incValue( 0 ) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( int32_t& data, int8_t validState ) throw()
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            data += m_incValue;
        }
        return m_returnResult;
    }
};

/////////////////////////////////////////////////////////////////
class ViewerInt64 : public ViewerBase, public Mp::Int64::Observer
{
public:
    ///
    Mp::Int64&  m_mp1;

    /// Constructor
    ViewerInt64( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::Int64& mp1 )
        :ViewerBase( myMbox, masterThread )
        , Mp::Int64::Observer( myMbox )
        , m_mp1( mp1 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerInt64(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Mp::Int64& modelPointThatChanged ) throw()
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerInt64(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerInt64(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= 2 )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

class RmwInt64 : public Mp::Int64::Client
{
public:
    ///
    int m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T m_returnResult;
    ///
    int64_t                         m_incValue;

public:
    ///
    RmwInt64():m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_incValue( 0 ) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( int64_t& data, int8_t validState ) throw()
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            data += m_incValue;
        }
        return m_returnResult;
    }
};

/////////////////////////////////////////////////////////////////
class ViewerUint64 : public ViewerBase, public Mp::Uint64::Observer
{
public:
    ///
    Mp::Uint64&  m_mp1;

    /// Constructor
    ViewerUint64( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::Uint64& mp1 )
        :ViewerBase( myMbox, masterThread )
        , Mp::Uint64::Observer( myMbox )
        , m_mp1( mp1 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerUint64(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Mp::Uint64& modelPointThatChanged ) throw()
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerUint64(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerUint64(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= 2 )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

class RmwUint64 : public Mp::Uint64::Client
{
public:
    ///
    int m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T m_returnResult;
    ///
    uint64_t                        m_incValue;

public:
    ///
    RmwUint64():m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_incValue( 0 ) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( uint64_t& data, int8_t validState ) throw()
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            data += m_incValue;
        }
        return m_returnResult;
    }
};

/////////////////////////////////////////////////////////////////
class ViewerBool : public ViewerBase, public Mp::Bool::Observer
{
public:
    ///
    Mp::Bool&  m_mp1;

    /// Constructor
    ViewerBool( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::Bool& mp1 )
        :ViewerBase( myMbox, masterThread )
        , Mp::Bool::Observer( myMbox )
        , m_mp1( mp1 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerBool(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Mp::Bool& modelPointThatChanged ) throw()
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerBool(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerBool(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= 2 )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

class RmwBool : public Mp::Bool::Client
{
public:
    ///
    int m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T m_returnResult;
    ///
    bool                            m_nextValue;

public:
    ///
    RmwBool():m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_nextValue( true ) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( bool& data, int8_t validState ) throw()
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            data = m_nextValue;
        }
        return m_returnResult;
    }
};


/////////////////////////////////////////////////////////////////
class ViewerFloat : public ViewerBase, public Mp::Float::Observer
{
public:
    ///
    Mp::Float&  m_mp1;

    /// Constructor
    ViewerFloat( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::Float& mp1 )
        :ViewerBase( myMbox, masterThread )
        , Mp::Float::Observer( myMbox )
        , m_mp1( mp1 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerFloat(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Mp::Float& modelPointThatChanged ) throw()
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerFloat(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerFloat(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= 2 )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

class RmwFloat : public Mp::Float::Client
{
public:
    ///
    int m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T m_returnResult;
    ///
    float                           m_incValue;

public:
    ///
    RmwFloat():m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_incValue( 0.0 ) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( float& data, int8_t validState ) throw()
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            data += m_incValue;
        }
        return m_returnResult;
    }
};

/////////////////////////////////////////////////////////////////
class ViewerDouble : public ViewerBase, public Mp::Double::Observer
{
public:
    ///
    Mp::Double&  m_mp1;

    /// Constructor
    ViewerDouble( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::Double& mp1 )
        :ViewerBase( myMbox, masterThread )
        , Mp::Double::Observer( myMbox )
        , m_mp1( mp1 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerDouble(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Mp::Double& modelPointThatChanged ) throw()
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerDouble(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerDouble(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= 2 )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

class RmwDouble : public Mp::Double::Client
{
public:
    ///
    int m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T m_returnResult;
    ///
    double                           m_incValue;

public:
    ///
    RmwDouble():m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_incValue( 0.0 ) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( double& data, int8_t validState ) throw()
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            data += m_incValue;
        }
        return m_returnResult;
    }
};

/////////////////////////////////////////////////////////////////
class ViewerString : public ViewerBase, public Mp::String::Observer
{
public:
    ///
    Mp::String&  m_mp1;

    /// Constructor
    ViewerString( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::String& mp1 )
        :ViewerBase( myMbox, masterThread )
        , Mp::String::Observer( myMbox )
        , m_mp1( mp1 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerString(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Mp::String& modelPointThatChanged ) throw()
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerString(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerString(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= 2 )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

class RmwString : public Mp::String::Client
{
public:
    ///
    int                             m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T m_returnResult;
    ///
    const char*                     m_valueToWrite;

public:
    ///
    RmwString():m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_valueToWrite( 0 ) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( Mp::String::Data& data, int8_t validState ) throw()
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            size_t newLen  = strlen( m_valueToWrite );
            newLen         = data.maxLength >= newLen ? newLen : data.maxLength;
            data.stringLen = newLen;
            strncpy( data.stringPtr, m_valueToWrite, newLen );
            data.stringPtr[newLen] = '\0';
        }
        return m_returnResult;
    }
};

/////////////////////////////////////////////////////////////////
class ViewerRefCounter : public ViewerBase, public Mp::RefCounter::Observer
{
public:
    ///
    Mp::RefCounter&  m_mp1;
    ///
    uint32_t         m_expectedNotifyCount;

    /// Constructor
    ViewerRefCounter( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::RefCounter& mp1, uint32_t expectedCount )
        :ViewerBase( myMbox, masterThread )
        , Mp::RefCounter::Observer( myMbox )
        , m_mp1( mp1 )
        , m_expectedNotifyCount( expectedCount )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerRefCounter(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Mp::RefCounter& modelPointThatChanged ) throw()
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerRefCounter(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerRefCounter(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= m_expectedNotifyCount )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

/////////////////////////////////////////////////////////////////
class ViewerArrayUint8 : public ViewerBase, public Mp::ArrayUint8::Observer
{
public:
    ///
    Mp::ArrayUint8&  m_mp1;

    /// Constructor
    ViewerArrayUint8( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::ArrayUint8& mp1 )
        :ViewerBase( myMbox, masterThread )
        , Mp::ArrayUint8::Observer( myMbox )
        , m_mp1( mp1 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerArrayUint8(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Mp::ArrayUint8& modelPointThatChanged ) throw()
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerArrayUint8(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerArrayUint8(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= 2 )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

class RmwArrayUint8 : public Mp::ArrayUint8::Client
{
public:
    ///
    int                              m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T  m_returnResult;
    ///
    unsigned                         m_index;
    ///
    uint8_t                          m_newValue;
public:
    ///
    RmwArrayUint8():m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_index( 0 ), m_newValue(0) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( Mp::ArrayUint8::Data& data, int8_t validState ) throw()
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE && m_index < data.numElements )
        {
            data.firstElemPtr[m_index] = m_newValue;
        }
        return m_returnResult;
    }
};


#endif