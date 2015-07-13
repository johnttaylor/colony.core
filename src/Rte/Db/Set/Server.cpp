#ifndef Rte_Db_Set_Server_h_
#define Rte_Db_Set_Server_h_
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

#include "Server.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/FatalError.h"


///
using namespace Rte::Db::Set;

#define SECT_       "Rte::Db::Set"


///////////////////////////////////////
Server::Server( Rte::Db::Record::Handler&                              recordLayer,
                Cpl::Itc::PostApi&                                     setAndRecordLayersMbox, 
                Cpl::Container::Dictionary<Rte::Db::Set::LocalApi_>&   sets
              )
:Cpl::Itc::CloseSync(setAndRecordLayersMbox)
,m_recordLayer(recordLayer)
,m_opened(false)
,m_conversion(false)
,m_setCount(0)
,m_openCount(0)
,m_closeCount(0)
,m_openMsgPtr(0)
,m_closeMsgPtr(0)
,m_sets(sets)
    {
    }


///////////////////////////////////////
void Server::defaultAllSetsContent() throw()
    {
    DefaultPayload                payload;
    Cpl::Itc::SyncReturnHandler   srh;
    DefaultMsg                    msg(*this,payload,srh);
    m_mbox.postSync(msg);
    }


void Server::request( DefaultMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::DefaultMsg (opened=%d", m_opened ) );

    // Ignore request if NOT opened, i.e. it would do anything anyway since all 
    // Sets are defaulted as part of the opening the DB sequence
    if ( m_opened )
        {
        // Default ALL registered sets
        Rte::Db::Set::LocalApi_* setPtr = m_sets.first();
        while( setPtr )
            {
            setPtr->defaultContent();
            setPtr = m_sets.next( *setPtr );
            }
        }

    msg.returnToSender();
    }

void Server::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::OpenMsg") );

    // Something is BAD if trying to open an already opened DB
    if ( m_opened )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Set::Server::request::OpenMsg - Protocol error." );
        }
    
    // Housekeeping
    m_conversion = false;
    m_setCount   = 0;
    m_openCount  = 0;
    m_openMsgPtr = &msg;

    // Start all registered sets 
    Rte::Db::Set::LocalApi_* setPtr = m_sets.first();
    while( setPtr )
        {
        m_openCount++;
        setPtr->start( m_recordLayer );
        setPtr = m_sets.next( *setPtr );
        }

    // Start the Record Layer
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::OpenMsg - set count=%d", m_openCount) );
    m_recordLayer.start();
    }

void Server::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::request::CloseMsg") );

    // Something is BAD if trying to close an already closed DB
    if ( !m_opened )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Set::Server::request::CloseMsg - Protocol error." );
        }

    // Housekeeping
    m_closeCount  = 0;
    m_closeMsgPtr = &msg;

    // Stop all registered Sets
    Rte::Db::Set::LocalApi_* setPtr = m_sets.first();
    while( setPtr )
        {
        m_closeCount++;
        setPtr->stop();
        setPtr = m_sets.next( *setPtr );
        }
    }


/////////////////////////////////
void Server::notifySetWaiting( LocalApi_& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::setWaiting") );
    m_setCount++;
    }

void Server::notifySetInitialized( LocalApi_& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::setInitialized") );
    m_setCount--;
    }

void Server::notifySetConverted( LocalApi_& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::setConverted") );
    m_conversion = true;
    }

void Server::notifySetStopped( LocalApi_& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::setStopped") );

    // Something is BAD if my set is stopped and the Set Handler has NOT be requested to stop
    if ( !m_closeMsgPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Set::Server::notifySetStopped - Protocol error." );
        }

    // Stop the Record Layer ONCE ALL Sets have stopped
    if ( --m_closeCount == 0 )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::setStopped - Set Layer Stopped") );
        m_recordLayer.stop();
        }
    }
    
void Server::notifySetStarted( LocalApi_& )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::setStarted. openCount=%d", m_openCount) );

    // Something is BAD if a Set is opening and the Set Handler has NOT be requested to open
    if ( !m_openMsgPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Set::Server::notifySetStarted - Protocol error." );
        }

    // Return my Open Request message once ALL sets have completed their opening sequence.
    if ( --m_openCount == 0 )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::notifySetStarted - Set Layer Started") );
        m_openMsgPtr->returnToSender();
        m_openMsgPtr = 0;
        m_opened     = true;
        }
    }

void Server::registerSet( LocalApi_& set )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Server::registerSet") );
    m_sets.insert( set );
    }


/////////////////////////////////
Rte::Db::Record::Api* Server::getRecordApi( const char* recName, uint16_t nameLen )
    {
    Cpl::Container::KeyStringBuffer recordNameKey( recName, nameLen );
    LocalApi_*                      setPtr = m_sets.find( recordNameKey );
    if ( !setPtr )
        {
        return 0;
        }

    return &(setPtr->getRecord());
    }

void Server::notifyOpenCompleted(void)
    {
    LocalApi_* setPtr = m_sets.first();
    while( setPtr )
        {
        setPtr->notifyLoadCompleted();
        setPtr = m_sets.next( *setPtr );
        }
    }

void Server::notifyOpenedWithErrors(void)
    {
    LocalApi_* setPtr = m_sets.first();
    while( setPtr )
        {
        setPtr->notifyLoadFailed();
        setPtr = m_sets.next( *setPtr );
        }
    }

bool Server::isCleanLoad(void)
    {
    return m_setCount == 0 && !m_conversion;
    }

void Server::notifyStopped()
    {
    // Something is BAD if the Record Layer has stopped and the Set Handler has NOT be requested to stop
    if ( !m_closeMsgPtr )
        {
        Cpl::System::FatalError::logf( "Rte::Db::Set::Server::notifyStopped - Protocol error." );
        }

    // Return/complete my close request
    m_closeMsgPtr->returnToSender();
    m_closeMsgPtr = 0;
    m_opened      = false;
    }


void Server::notifyIncompatible(void)
    {
    // TODO:  Not sure what goes HERE!!!
    }

