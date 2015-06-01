/*----------------------------------------------------------------------------- 
* This file is part of the Shift-Right Open Repository.  The Repository is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the root directory or on the Internet at           
* http://www.shift-right.com/openrepo/license.htm                               
*                                                                               
* Copyright (c) 2001-2011 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Base.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/SyncReturnHandler.h"


using namespace Rte::Point::Model;

#define SECT_   "Rte::Point::Model"


///////////////////
Base::Base( Rte::Point::Api& myPoint, Cpl::Itc::PostApi& myMbox )
:m_myPoint(myPoint)
//,m_viewerSAP(*this,myMbox)
,m_controllerSAP(*this,myMbox)
,m_querySAP(*this,myMbox)
    {
    }




///////////////////
//Rte::Point::Model::ViewerRequest::SAP&     
//Base::getViewerSAP(void)
//    {
//    return m_viewerSAP;
//    }

Rte::Point::Model::ControllerRequest::SAP& 
Base::getControllerSAP(void)
    {
    return m_controllerSAP;
    }

Rte::Point::Model::QueryRequest::SAP& 
Base::getQuerySAP(void)
    {
    return m_querySAP;
    }



///////////////////
void Base::update( Rte::Point::Api& controllerPoint )
    {
    UpdatePayload                 payload( controllerPoint );
    Cpl::Itc::SyncReturnHandler   srh;
    UpdateMsg                     msg(*this,payload,srh);
    m_controllerSAP.postSync(msg);
    }

void Base::update( Rte::Tuple::Api& controllerTuple, unsigned tupleIdx )
    {
    UpdateTuplePayload            payload( controllerTuple, tupleIdx );
    Cpl::Itc::SyncReturnHandler   srh;
    UpdateTupleMsg                msg(*this,payload,srh);
    m_controllerSAP.postSync(msg);
    }

void Base::query( Rte::Point::Api& dstPoint )
    {
    QueryPayload                  payload( dstPoint );
    Cpl::Itc::SyncReturnHandler   srh;
    QueryMsg                      msg(*this,payload,srh);
    m_querySAP.postSync(msg);
    }

void Base::query( Rte::Tuple::Api& dstTuple, unsigned tupleIdx )
    {
    QueryTuplePayload             payload( dstTuple, tupleIdx );
    Cpl::Itc::SyncReturnHandler   srh;
    QueryTupleMsg                 msg(*this,payload,srh);
    m_querySAP.postSync(msg);
    }



///////////////////
//void Base::request( AttachMsg& msg )
//    {
//    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(AttachMsg) - (%p) viewer=%p (msgP=%p)", this, &(msg.getPayload()._point), &msg ));
//
//    // If different -->make a copy and return immediately
//    if ( m_myPoint.compareAndCopy(msg.getPayload()._point) )
//        {
//        msg.returnToSender();
//        }
//
//    // Hold the notification request until the tuple changes
//    else
//        {
//        m_pendingNotify.put( &msg );
//        }
//    }
//    
//void Base::request( DetachMsg& msg ) 
//    {
//    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(DetachMsg) - (%p) msgToDetach=%p", this, &(msg.getPayload()._msgToDetachP) ));
//
//    // Remove the pending change notification if there is one
//    // NOTE: I intentionally do NOT return a pending Attach message, but
//    //       simply drop my reference to it.  This is to maintain the
//    //       sematics that when the client's response() method for the
//    //       Attach Message is called - it is due to change and ONLY a 
//    //       change in state of Point.
//    m_pendingNotify.remove( msg.getPayload()._msgToDetachP );
//    msg.returnToSender();
//    }
//
//void Base::request( ForceMsg& msg ) 
//    {
//    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(ForceMsg) - (%p) msgToForce=%p", this, &(msg.getPayload()._msgToForceP) ));
//
//    // Remove the viewer message from my list of pending notificatinos
//    AttachMsg* viewerMsgP = msg.getPayload()._msgToForceP;
//    if ( m_pendingNotify.remove(viewerMsgP) )
//        {
//        // Force an update to the viewer point (this is important
//        // if there are any Delta/Threshold element types being viewed)
//        viewerMsgP->getPayload()._point.copyFrom( m_myPoint,  &(viewerMsgP->getPayload()._point) );
//
//        // Generate the change notification
//        viewerMsgP->returnToSender();
//        }
//
//    // Return the force message
//    msg.returnToSender();
//    }


///////////////////
void Base::request( UpdateMsg& msg ) 
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(UpdateMsg) - (%p) src=%p", this, &(msg.getPayload().m_srcPoint) ));

    // Update the Point
    m_myPoint.copyFrom( msg.getPayload().m_srcPoint, &msg.getPayload().m_srcPoint );
    checkForNotifications();
    msg.returnToSender();
    }

void Base::request( UpdateTupleMsg& msg ) 
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(UpdateTupleMsg) - (%p) srcTuple=%p, tupleIdx=%u", this, &(msg.getPayload().m_srcTuple), msg.getPayload().m_tupleIdx ));

    // Update the Tuple
    Rte::Tuple::Api::copy( m_myPoint.getTuple(msg.getPayload().m_tupleIdx), msg.getPayload().m_srcTuple, &(msg.getPayload().m_srcTuple) );
    
    // Increment my Point's seqeuence number since something changed
    m_myPoint.incrementSequenceNumber();
    m_myPoint.setUpdatedState();

    // Process any change notifications
    checkForNotifications();
    msg.returnToSender();
    }

//void Base::request( QueryAndModifyPointMsg& msg ) 
//    {
//    // Do: Read
//    msg.getPayload()._point.copyFrom( m_myPoint, 0 ); // Note: Read ALL elements
//
//    // Do: Modify
//    Rte::Point::Api* modifiedPoint = 0;
//    msg.getPayload()._modifyCallback.modify( modifiedPoint );
//
//    // Do: Write
//    if ( modifiedPoint )
//        {
//        m_myPoint.copyFrom( *modifiedPoint, modifiedPoint );
//        }
//
//    // Notify the viewers if neccesary
//    checkForNotifications();
//    msg.returnToSender();
//    }
//
//
//void Base::request( QueryAndModifyContainerMsg& msg ) 
//    {
//    // Proceass ALL tuples items (but not the sequence number tuple)
//    int j = msg.getPayload()._indexStart;
//    for(;;)
//        {
//        // Do: Read
//        Eden::MVC::Tuple::Api::copy( msg.getPayload()._tuple, m_myPoint.getTuple(j), 0 ); // Note: Read ALL elements
//
//        // Do: Modify
//        Eden::MVC::Tuple::Api* modifiedTuple = 0;
//        int  next     = msg.getPayload()._modifyCallback.item( j, modifiedTuple );
//
//        // Do: Write
//        if ( modifiedTuple )
//            {
//            Eden::MVC::Tuple::Api::copy( m_myPoint.getTuple(j), *modifiedTuple, modifiedTuple );
//            }
//
//        // Auto increment the tuple index 
//        if ( next == Eden::MVC::Point::Controller::QueryAndModify::ModifyContainerApi::_NEXT )
//            {
//            j++;
//            }
//
//        // Exit if requested by the controller
//        else if ( next == Eden::MVC::Point::Controller::QueryAndModify::ModifyContainerApi::_DONE ) 
//            {
//            break;
//            }
//
//        // Controller driving the tuple sequence
//        else 
//            {
//            j = next;
//            }
//
//        // Exit if end-of-items is reached 
//        if ( j >= m_myPoint.getNumTuples()-1 )
//            {
//            break;
//            }
//        }
//
//    // Update the sequence number to process any membership changes
//    Eden::MVC::Tuple::Api::copy( m_myPoint.getTuple(m_myPoint.getNumTuples()-1), msg.getPayload()._seqNum, &(msg.getPayload()._seqNum) );
//
//    // Notify the viewers if neccesary
//    checkForNotifications();
//    msg.returnToSender();
//    }


void Base::checkForNotifications(void)
    {
    // Walk the pending Notification list looking for changes
    // NOTE: I create a temporary list to hold the non-changed
    //       items in hopes of making the process as efficient as
    //       possible since the pending Notification list is only
    //       a singly-linked list.
//    Cpl::Container::SList<AttachMsg> tempList;
//    AttachMsg*                       ptr = m_pendingNotify.get();
//    while( ptr )
//        {
//        if ( m_myPoint.compareAndCopy(ptr->getPayload()._point) )
//            {
//            ptr->returnToSender();
//            }
//        else
//            {
//            tempList.put( *ptr );
//            }
//        ptr = m_pendingNotify.get();
//        }
//
//    // Restore the pending list
//    tempList.move( m_pendingNotify );
    }


///////////////////
void Base::request( QueryMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(QueryMsg) - (%p) dst=%p", this, &(msg.getPayload().m_dstPoint) ));

    // Brute force update (no isDifferent() check) the client's 'point' and return immediately
    msg.getPayload().m_dstPoint.copyFrom( m_myPoint, &(msg.getPayload().m_dstPoint) );
    msg.returnToSender();
    }
    
void Base::request( QueryTupleMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(QueryTupleMsg) - (%p) idx=%d, dst=%p", this, msg.getPayload().m_tupleIdx, &(msg.getPayload().m_dstTuple) ));

    // Brute force update (no isDifferent() check) the client's 'tuple' 
    unsigned index = msg.getPayload().m_tupleIdx;
    Rte::Tuple::Api::copy( msg.getPayload().m_dstTuple, m_myPoint.getTuple(index), &(msg.getPayload().m_dstTuple) );

//    // If a search callback was specified, iterate through all tuple in the CONTAINER
//    if ( msg.getPayload()._searchCallback )
//        {
//        for(;;)
//            {
//            // Callback the client and exit if the client has 'found' what it is searching for
//            if ( msg.getPayload()._searchCallback->item(index) == Jcl::TraverserPattern::_abortTraversal )
//                {
//                break;
//                }
//
//            // Check if I am at the end of the Tuples.  Since in theory I am 
//            // walking the tuples in a Container - I need to SKIP container's 
//            // sequence number tuple.
//            if ( ++index >= m_myPoint.getNumTuples() - 1 )
//                {
//                break;
//                }
//
//            // Load the next tuple
//            Eden::MVC::Tuple::Api::copy( msg.getPayload().m_dstTuple, m_myPoint.getTuple(index), &(msg.getPayload().m_dstTuple) );
//            }
//        }
            
    msg.returnToSender();
    }
    
