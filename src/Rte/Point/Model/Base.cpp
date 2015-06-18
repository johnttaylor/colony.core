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

#include "Base.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/SyncReturnHandler.h"


using namespace Rte::Point::Model;

#define SECT_   "Rte::Point::Model"


///////////////////
Base::Base( Rte::Point::Api& myPoint, Cpl::Itc::PostApi& myMbox )
:m_myPoint(myPoint)
,m_viewerSAP(*this,myMbox)
,m_controllerSAP(*this,myMbox)
,m_querySAP(*this,myMbox)
    {
    }




///////////////////
Rte::Point::Model::ViewerRequest::SAP&     
Base::getViewerSAP(void)
    {
    return m_viewerSAP;
    }

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

void Base::update( Rte::Tuple::Api& controllerTuple, unsigned tupleIdx, bool membershipChanged  )
    {
    UpdateTuplePayload            payload( controllerTuple, tupleIdx, membershipChanged );
    Cpl::Itc::SyncReturnHandler   srh;
    UpdateTupleMsg                msg(*this,payload,srh);
    m_controllerSAP.postSync(msg);
    }


void Base::update( Rte::Point::Api& rmwPoint, Rte::Point::Controller::RmwClient& modifyCallback )
    {
    RmwPayload                    payload( rmwPoint, modifyCallback );
    Cpl::Itc::SyncReturnHandler   srh;
    RmwMsg                        msg(*this,payload,srh);
    m_controllerSAP.postSync(msg);
    }

void Base::update( Rte::Tuple::Api&                            rmwTuple,
                   bool                                        membershipChanged,
                   Rte::Point::Controller::RmwContainerClient& modifyCallback,
                   unsigned                                    itemIndexStart
                 )
    {
    RmwContainerPayload           payload( rmwTuple, membershipChanged, modifyCallback, itemIndexStart );
    Cpl::Itc::SyncReturnHandler   srh;
    RmwContainerMsg               msg(*this,payload,srh);
    m_controllerSAP.postSync(msg);
    }


///////////////////
void Base::query( Rte::Point::Api& dstPoint, QueryRequest::Option_T copyOption  )
    {
    QueryPayload                  payload( dstPoint, copyOption );
    Cpl::Itc::SyncReturnHandler   srh;
    QueryMsg                      msg(*this,payload,srh);
    m_querySAP.postSync(msg);
    }

void Base::query( Rte::Tuple::Api& dstTuple, unsigned tupleIdx, Rte::Point::Query::Traverser* walkCallbackPtr, QueryRequest::Option_T copyOption )
    {
    QueryTuplePayload             payload( dstTuple, tupleIdx, walkCallbackPtr, copyOption );
    Cpl::Itc::SyncReturnHandler   srh;
    QueryTupleMsg                 msg(*this,payload,srh);
    m_querySAP.postSync(msg);
    }


///////////////////
void Base::pollViewer( ViewerRequest::RegisterMsg& viewerToPoll )
    {
    PollPayload                   payload( viewerToPoll );
    Cpl::Itc::SyncReturnHandler   srh;
    PollMsg                       msg(*this,payload,srh);
    m_viewerSAP.postSync(msg);
    }


///////////////////
void Base::request( RegisterMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(RegisterMsg) - (%p) viewer=%p (msgP=%p)", this, &(msg.getPayload().m_viewerPoint), &msg ));

    // If different -->make a copy and return immediately
    if ( m_myPoint.compareAndCopy(msg.getPayload().m_viewerPoint, false, msg.getPayload().m_compareValues) )
        {
        msg.returnToSender();
        }

    // Hold the notification request until the tuple changes
    else
        {
        m_pendingNotify.put( msg );
        }
    }
    
void Base::request( CancelMsg& msg ) 
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(CancelMsg) - (%p) msgToCancel=%p", this, &(msg.getPayload().m_msgToCancel) ));

    // Remove the pending change notification if there is one
    // NOTE: I intentionally do NOT return a pending Register message, but
    //       simply drop my reference to it.  This is to maintain the
    //       sematics that when the client's response() method for the
    //       Register Message is called - it is due to a change and ONLY a 
    //       change in state of Point.
    m_pendingNotify.remove( msg.getPayload().m_msgToCancel );
    msg.returnToSender();
    }

void Base::request( PollMsg& msg ) 
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(PollMsg) - (%p) msgToPoll=%p", this, &(msg.getPayload().m_msgToPoll) ));

    // Remove the viewer message from my list of pending notificatinos
    RegisterMsg& viewerMsg = msg.getPayload().m_msgToPoll;
    if ( m_pendingNotify.remove(viewerMsg) )
        {
        // Force an update to the viewer point (this is important
        // if there are any Delta/Threshold Element types being viewed)
        viewerMsg.getPayload().m_viewerPoint.copyFrom( m_myPoint,  &(viewerMsg.getPayload().m_viewerPoint) );
       
        // Update the Client's (i.e. the Viewer's) Point sequence number to match the Model's Point sequence number
        viewerMsg.getPayload().m_viewerPoint.setSequenceNumber( m_myPoint.getSequenceNumber() );

        // Generate the change notification
        viewerMsg.returnToSender();
        }

    // Return the force message
    msg.returnToSender();
    }


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
    
    // Detect membership updates (from the Controller)
    if ( msg.getPayload().m_membershipChanged )
        {
        m_myPoint.incrementSequenceNumber();
        }

    // Process any change notifications
    checkForNotifications();
    msg.returnToSender();
    }


///////////////////
void Base::request( RmwMsg& msg ) 
    {
    // DO: Read
    msg.getPayload().m_clientPoint.copyFrom( m_myPoint, 0 ); // Force a read of ALL Tuples/Elements

    // DO: Modify
    Rte::Point::Api* modifiedPoint = 0;
    msg.getPayload().m_clientCb.modify( modifiedPoint );

    // DO: Write (if any)
    if ( modifiedPoint )
        {
        m_myPoint.copyFrom( *modifiedPoint, modifiedPoint );
        checkForNotifications();
        }

    msg.returnToSender();
    }


void Base::request( RmwContainerMsg& msg ) 
    {
    msg.returnToSender();
    }
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
    RegisterMsg* ptr = m_pendingNotify.first();
    while( ptr )
        {
        RegisterMsg* nextPtr = m_pendingNotify.next( *ptr );
         
        if ( m_myPoint.compareAndCopy(ptr->getPayload().m_viewerPoint, false, ptr->getPayload().m_compareValues) )
            {
            m_pendingNotify.remove( *ptr );
            ptr->returnToSender();
            }

        ptr = nextPtr;
        }
    }


///////////////////
void Base::request( QueryMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(QueryMsg) - (%p) dst=%p", this, &(msg.getPayload().m_dstPoint) ));

    // Brute force update (no isDifferent() check) the client's 'point' and return immediately
    if ( msg.getPayload().m_copyOption == eCOPY )
        {
        msg.getPayload().m_dstPoint.copyFrom( m_myPoint, &(msg.getPayload().m_dstPoint) );

        // Update the Client's (i.e. the Querier's) Point sequence number to match the Model's Point sequence number
        msg.getPayload().m_dstPoint.setSequenceNumber( m_myPoint.getSequenceNumber() );
        }

    // Use the compare-n-copy method to set the 'is different' status/state of the client's Query Point
    else    
        {
        m_myPoint.compareAndCopy(msg.getPayload().m_dstPoint, false, msg.getPayload().m_copyOption == eCOMPARE_VALUES_AND_COPY );
        }


    // Return the client's message
    msg.returnToSender();
    }
    

void Base::request( QueryTupleMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(QueryTupleMsg) - (%p) idx=%d, dst=%p, callback=%p", this, msg.getPayload().m_tupleIdx, &(msg.getPayload().m_dstTuple), msg.getPayload().m_callbackPtr ));

    // Copy the Model's tuple to the Query Tuple
    unsigned index = msg.getPayload().m_tupleIdx;
    copyTuple( msg, index );

    // If a search callback was specified, iterate through all tuple in the CONTAINER
    if ( msg.getPayload().m_callbackPtr )
        {
        for(;;)
            {
            // Callback the client and exit if the client has 'found' what it is searching for
            if ( msg.getPayload().m_callbackPtr->item(index) == Cpl::Type::Traverser::eABORT )
                {
                break;
                }

            // Check if I am at the end of the Tuples. 
            if ( ++index >= m_myPoint.getNumTuples() )
                {
                break;
                }

            // Load the next tuple
            copyTuple( msg, index );
            }
        }
            
    msg.returnToSender();
    }
    

void Base::copyTuple( QueryTupleMsg& msg, unsigned index )
    {
    // Brute force update (no isDifferent() check) the client's 'tuple' 
    if ( msg.getPayload().m_copyOption == eCOPY )
        {
        Rte::Tuple::Api::copy( msg.getPayload().m_dstTuple, m_myPoint.getTuple(index), &(msg.getPayload().m_dstTuple) );
        
        // Update the Client's (i.e. the Querier's) Tuple sequence number to match the Model's Tuple sequence number
        msg.getPayload().m_dstTuple.setSequenceNumber( m_myPoint.getTuple(index).getSequenceNumber() );
        }

    // Use the compare-n-copy method to set the 'is different' status/state of the client's Query Tuple
    else
        {
        Rte::Tuple::Api::compareAndCopy( m_myPoint.getTuple(index), msg.getPayload().m_dstTuple, false,  msg.getPayload().m_copyOption == eCOMPARE_VALUES_AND_COPY );
        }
    }
