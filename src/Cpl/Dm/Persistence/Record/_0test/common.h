#ifndef common_h_
#define common_h_
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

#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/RmwComposer.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Dm/Persistence/Record/MpServerStatus.h"
#include "Cpl/Dm/Persistence/Record/Base.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/Mp/String.h"
#include "Cpl/Dm/Persistence/Chunk/Null.h"

/// 
using namespace Cpl::Dm;

#define SECT_   "_0test"


/////////////////////////////////////////////////////////////////

class MyRecord : public Cpl::Dm::Persistence::Record::Base
{
public:
    ///
    SubscriberComposer<MyRecord, Cpl::Dm::Mp::String>  m_observer1;
    ///
    SubscriberComposer<MyRecord, Cpl::Dm::Mp::String>  m_observer2;
    ///
    SubscriberComposer<MyRecord, Cpl::Dm::Mp::String>  m_observer3;
    ///
    ModelPointReference_t   m_mpRef1;
    ///
    ModelPointReference_t   m_mpRef2;
    ///
    ModelPointReference_t   m_mpRef3;

    /// Note: For an 'real' Application the following 'members' are typically hardwire into the concrete class, i.e. typically not data members
    ///
    Cpl::Dm::Mp::String&   m_mp1;
    ///
    Cpl::Dm::Mp::String&   m_mp2;
    ///
    Cpl::Dm::Mp::String&   m_mp3;
    ///
    const char*             m_default1;
    ///
    const char*             m_default2;
    ///
    const char*             m_default3;

public:
    /// Constructor.  Note: Record names must be unique within a Model Database
    MyRecord( Cpl::Container::Map<Api_>&     myRecordList,
              unsigned long                  delayWriteTimeInMsec,
              const char*                    name,
              Cpl::Dm::MailboxServer&        recordLayerMbox,
              Cpl::Dm::Mp::String&           modelPoint1,
              Cpl::Dm::Mp::String&           modelPoint2,
              Cpl::Dm::Mp::String&           modelPoint3,
              const char*                    defaultValueMp1 = "MP1 default",
              const char*                    defaultValueMp2 = "MP2 default",
              const char*                    defaultValueMp3 = "MP3 default"
    )
        :Base( myRecordList, delayWriteTimeInMsec, name, recordLayerMbox )
        , m_observer1( recordLayerMbox, *this, &MyRecord::modelPointNChanged )
        , m_observer2( recordLayerMbox, *this, &MyRecord::modelPointNChanged )
        , m_observer3( recordLayerMbox, *this, &MyRecord::modelPointNChanged )
        , m_mpRef1( modelPoint1 )
        , m_mpRef2( modelPoint2 )
        , m_mpRef3( modelPoint3 )
        , m_mp1( modelPoint1 )
        , m_mp2( modelPoint2 )
        , m_mp3( modelPoint3 )
        , m_default1( defaultValueMp1 )
        , m_default2( defaultValueMp2 )
        , m_default3( defaultValueMp3 )
    {
        registerModelPoint( m_mpRef1 );
        registerModelPoint( m_mpRef2 );
        registerModelPoint( m_mpRef3 );
    }

    /// FSM Action
    void connectToModel() noexcept
    {
        m_mp1.attach( m_observer1, m_mp1.getSequenceNumber() );
        m_mp2.attach( m_observer2, m_mp2.getSequenceNumber() );
        m_mp3.attach( m_observer3, m_mp3.getSequenceNumber() );
    }

    /// FSM Action
    void defaultData() noexcept
    {
        m_mp1.write( m_default1 );
        m_mp2.write( m_default2 );
        m_mp3.write( m_default3 );
    }


    /// FSM Action
    void disconnectFromModel() noexcept
    {
        m_mp1.detach( m_observer1 );
        m_mp2.detach( m_observer2 );
        m_mp3.detach( m_observer3 );
    }

    // Change notification
    void modelPointNChanged( Cpl::Dm::Mp::String& modelPointThatChanged ) noexcept
    {
        generateEvent( Cpl::Dm::Persistence::Record::Fsm_evDataModified );
    }

};

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

class ViewerMpServerStatus : public ViewerBase, public Persistence::Record::MpServerStatus::Observer
{
public:
    ///
    Persistence::Record::MpServerStatus & m_mp1;

    /// Constructor
    ViewerMpServerStatus( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Persistence::Record::MpServerStatus& mp1 )
        :ViewerBase( myMbox, masterThread )
        , Persistence::Record::MpServerStatus::Observer( myMbox )
        , m_mp1( mp1 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerMpServerStatus(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void subscribe() { m_mp1.attach( *this ); }
    ///
    void unsubscribe() { m_mp1.detach( *this ); }
    ///
    void modelPointChanged( Persistence::Record::MpServerStatus& modelPointThatChanged ) noexcept
    {
        if ( m_done != true )
        {
            m_notifCount++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ViewerMpServerStatus(%p) Changed!: count=%lu", this, (unsigned long) m_notifCount) );

            m_lastSeqNumber  = modelPointThatChanged.getSequenceNumber();
            m_lastValidState = modelPointThatChanged.getValidState();

            if ( m_pendingOpenMsgPtr != 0 && m_notifCount == 1 )
            {
                m_pendingOpenMsgPtr->returnToSender();
                m_opened            = true;
                m_pendingOpenMsgPtr = 0;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("..ViewerMpServerStatus(%p) Returning Open Msg.") );
            }

            if ( m_notifCount >= 2 )
            {
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

class RmwServerStatus : public Persistence::Record::MpServerStatus::Client
{
public:
    ///
    int                             m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T m_returnResult;
    ///
    int                             m_incValue;

public:
    ///
    RmwServerStatus():m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_incValue( 0 ) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( Persistence::Record::ServerStatus& data, int8_t validState ) noexcept
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            data = Persistence::Record::ServerStatus::_from_integral_unchecked( data + m_incValue );
        }
        return m_returnResult;
    }
};

class MyNullMedia : public Cpl::Dm::Persistence::Chunk::Null
{
public:
    /// Constructor
    MyNullMedia() {}


public:
    /// See Rte::Db::Chunk::Media
    Cpl::Io::File::InputOutputApi* openFile( bool& newfile ) noexcept
    {
        return 0;
    }
};
#endif