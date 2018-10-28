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
#include "Cpl/Rte/Persistence/Record/MpServerStatus.h"

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
    void modelPointChanged( Persistence::Record::MpServerStatus& modelPointThatChanged ) throw()
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
    ModelPoint::RmwCallbackResult_T callback( Persistence::Record::ServerStatus& data, int8_t validState ) throw()
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            data = Persistence::Record::ServerStatus::_from_integral_unchecked( data + m_incValue );
        }
        return m_returnResult;
    }
};


#endif