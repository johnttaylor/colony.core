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
#include "Cpl/Text/strip.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"


using namespace Rte::Point::Model;

#define SECT_   "Rte::Point::Model"


///////////////////
Base::Base( Rte::Point::Api& myPoint, Cpl::Itc::PostApi& myMbox )
:m_myPoint(myPoint)
,m_viewerSAP(*this,myMbox)
,m_controllerSAP(*this,myMbox)
,m_querySAP(*this,myMbox)
,m_defaultSAP(*this,myMbox)
    {
    m_myPoint.setAsModelPoint_();
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

Rte::Point::Model::DefaultRequest::SAP& 
Base::getDefaultSAP(void)
    {
    return m_defaultSAP;
    }


///////////////////
void Base::defaultContents() throw()
    {
    DefaultPayload                payload;
    Cpl::Itc::SyncReturnHandler   srh;
    DefaultMsg                    msg(*this,payload,srh);
    m_defaultSAP.postSync(msg);
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
                   Rte::Point::Controller::RmwContainerClient& modifyCallback,
                   unsigned                                    itemIndexStart
                 )
    {
    RmwContainerPayload           payload( rmwTuple, modifyCallback, itemIndexStart );
    Cpl::Itc::SyncReturnHandler   srh;
    RmwContainerMsg               msg(*this,payload,srh);
    m_controllerSAP.postSync(msg);
    }


bool Base::update( Cpl::Text::String& sourceValues, int tupleIdx )
    {
    UpdateTextPayload             payload( sourceValues, tupleIdx );
    Cpl::Itc::SyncReturnHandler   srh;
    UpdateTextMsg                 msg(*this,payload,srh);
    m_controllerSAP.postSync(msg);

    return payload.m_success;
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


bool Base::query( Cpl::Text::String& results, int tupleIdx )
    {
    QueryTextPayload              payload( results, tupleIdx );
    Cpl::Itc::SyncReturnHandler   srh;
    QueryTextMsg                  msg(*this,payload,srh);
    m_querySAP.postSync(msg);

    return payload.m_success;
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
void Base::defaultContents_nonThreadSafe( void ) throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::defaultContents_nonThreadSafe() - (%p)", this ));

    // The default default is to mark all Tuples/Elements INVALID
    m_myPoint.setAllValidState( RTE_ELEMENT_API_STATE_INVALID );

    // Call the Application supplied default method for this set 
    defaultMe();

    // Mark the point as modified/updated
    touch_nonThreadSafe();

    // Process any change notifications
    checkForNotifications();
    }


Rte::Point::Api& Base::getMyPoint_nonThreadSafe( void ) throw()
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::getMyPoint_nonThreadSafe() - (%p)", this ));
    return m_myPoint;
    }
    

void Base::touch_nonThreadSafe( void ) throw()
    {
    // Increment the Tuples sequence numbers
    unsigned j;
    for(j=0; j<m_myPoint.getNumTuples(); j++)
        {
        m_myPoint.getTuple(j).setUpdatedState();
        m_myPoint.getTuple(j).incrementSequenceNumber();
        }

    // Increment the Point's sequence number, i.e. force a membership change (which in theory if I am a Container Point sorta of by defintion defaulting the Point is membership change)
    m_myPoint.incrementSequenceNumber();
    }


void Base::request( DefaultMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(DefaultMsg) - (%p)", this ));
    defaultContents_nonThreadSafe();
    msg.returnToSender();
    }


void Base::defaultMe( void )
    {
    // Do nothing
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
  
    // Process any change notifications
    checkForNotifications();

    // Update the Client's (i.e. the Controller's) Point sequence numbers to match the Model's Point sequence numbers (required for supporting Light Weight Viewers)
    msg.getPayload().m_srcPoint.copyAllSequenceNumbersFrom( m_myPoint );
    msg.returnToSender();
    }

void Base::request( UpdateTupleMsg& msg ) 
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(UpdateTupleMsg) - (%p) srcTuple=%p, tupleIdx=%u", this, &(msg.getPayload().m_srcTuple), msg.getPayload().m_tupleIdx ));

    // Update the Tuple
    Rte::Tuple::Api::copy( m_myPoint.getTuple( msg.getPayload().m_tupleIdx ), msg.getPayload().m_srcTuple, &(msg.getPayload().m_srcTuple) );
    
    // Detect membership updates (from the Controller)
    if ( msg.getPayload().m_membershipChanged )
        {
        m_myPoint.incrementSequenceNumber();
        }

    // Process any change notifications
    checkForNotifications();
  
    // Update the Client's (i.e. the Controllers's) Tuple sequence number to match the Model's Tuple sequence number (required for supporting Light Weight Viewers)
    msg.getPayload().m_srcTuple.setSequenceNumber( m_myPoint.getTuple( msg.getPayload().m_tupleIdx ).getSequenceNumber() );
    msg.returnToSender();
    }


///////////////////
void Base::request( RmwMsg& msg ) 
    {
    // DO: Read
    msg.getPayload().m_clientPoint.copyFrom( m_myPoint, 0 ); // Force a read of ALL Tuples/Elements

    // DO: Modify
    Rte::Point::Api* modifiedPointPtr = 0;
    msg.getPayload().m_clientCb.modify( modifiedPointPtr );

    // DO: Write (if any)
    if ( modifiedPointPtr )
        {
        m_myPoint.copyFrom( *modifiedPointPtr, modifiedPointPtr );
        
        // Process any change notifications
        checkForNotifications();

        // Update the Client's (i.e. the Controller's) Point sequence numbers to match the Model's Point sequence numbers (required for supporting Light Weight Viewers)
        modifiedPointPtr->copyAllSequenceNumbersFrom( m_myPoint );
        }

    msg.returnToSender();
    }


void Base::request( RmwContainerMsg& msg ) 
    {
    // Proceass ALL tuples items 
    bool     membershipChanged = false;
    unsigned j                 = msg.getPayload().m_tupleIdx;
    for(;;)
        {
        // DO: Read
        Rte::Tuple::Api::copy( msg.getPayload().m_clientTuple, m_myPoint.getTuple(j), 0 ); // Note: Read ALL elements

        // DO: Modify
        Rte::Tuple::Api* modifiedTuplePtr = 0;
        bool             membershipFlag   = false;
        int              next             = msg.getPayload().m_clientCb.modifyItem( j, modifiedTuplePtr, membershipFlag );

        // Process Membership changes
        if ( membershipFlag )
            {
            membershipChanged = true;
            }

        // DO: Write
        if ( modifiedTuplePtr )
            {
            Rte::Tuple::Api::copy( m_myPoint.getTuple(j), *modifiedTuplePtr, modifiedTuplePtr );
            
            // Update the Client's (i.e. the Controllers's) Tuple sequence number to match the Model's Tuple sequence number (required for supporting Light Weight Viewers)
            modifiedTuplePtr->setSequenceNumber( m_myPoint.getTuple(j).getSequenceNumber() );
            }

        // Auto increment the tuple index 
        if ( next == Rte::Point::Controller::RmwContainerClient::eNEXT )
            {
            if ( ++j >= m_myPoint.getNumTuples() )
                {
                break;
                }
            }

        // Controller driving the tuple sequence
        else if ( next >= 0 && ((unsigned) next) < m_myPoint.getNumTuples() )
            {
            j = next;
            }

        // Exit if end-of-items is reached (either eDONE or invalid return value)
        else
            {
            break;
            }
        }
                                                   
    // Detect membership changesfrom the Controller
    if ( membershipChanged )
        {
        m_myPoint.incrementSequenceNumber();
        }

    // Notify the viewers if neccesary
    checkForNotifications();
    msg.returnToSender();
    }


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

        // Update the Client's (i.e. the Querier's) Point sequence numbers to match the Model's Point sequence numbers
        msg.getPayload().m_dstPoint.copyAllSequenceNumbersFrom( m_myPoint );
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


///////////////////
void Base::request( QueryTextMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(QueryTextMsg) - (%p)", this) );

    Cpl::Text::String& results = msg.getPayload().m_results;

    // Do Tuple query
    if ( msg.getPayload().m_tupleIdx >= 0 )
        {
        // Trap: Tuple index out-of-range
        if ( (unsigned)(msg.getPayload().m_tupleIdx) >= m_myPoint.getNumTuples() )
            {
            msg.getPayload().m_success = false;
            }

        // Get tuple data
        else
            { 
            results.clear();
            copyTupleAsText( results, (unsigned) msg.getPayload().m_tupleIdx );
            }
        } 

    // Query entire Point
    else
        {
        results  = "{ ";
        unsigned i;
        for(i=0; i<m_myPoint.getNumTuples(); i++)
            {
            // Add seperator between tuples
            if ( i>0 )
                {
                results += ", ";
                }

            copyTupleAsText( results, i );
            } 
        results += " }";
        }

    // Return the client's message
    msg.returnToSender();
    }


void Base::copyTupleAsText( Cpl::Text::String& results, unsigned tupleIdx )
    {
    // Add tuple prefix
    results += "("; 

    // Loop through ALL elements
    Rte::Tuple::Api& tuple       = m_myPoint.getTuple(tupleIdx);
    bool             isContainer = m_myPoint.isContainer();
    unsigned         i;
    for(i=0; i<tuple.getNumElements(); i++)
        {
        Rte::Element::Api& element = tuple.getElement(i);

        // Add seperator between Elements
        if ( i>0 )
            {
            results += ", ";
            }

        // Convert the element's value to text
        element.toString( results, true );
        }

    // Add tuple postfix
    results += ")"; 
    }


///////////////////
void Base::request( UpdateTextMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::request(UpdateTextMsg) - (%p)", this) );

    // Housekeeping (default to FAILED update)
    msg.getPayload().m_success = false;
    const char* source = msg.getPayload().m_source;

    // Do Tuple update
    if ( msg.getPayload().m_tupleIdx >= 0 )
        {
        // Get tuple data
        if ( (unsigned)(msg.getPayload().m_tupleIdx) < m_myPoint.getNumTuples() )
            {
            if ( setTupleFromText( source, (unsigned) msg.getPayload().m_tupleIdx ) )
                {
                msg.getPayload().m_success = true;
                }
            }
        } 

    // Update entire Point
    else
        {
        source = Cpl::Text::stripSpace( source );
        if ( *source++ == '{' )
            {
            bool     parseError = false;
            unsigned i;
            for(i=0; i<m_myPoint.getNumTuples(); i++)
                {
                // Skip 'empty field' tuple(s)
                source = Cpl::Text::stripSpace( source );
                if ( *source == ',' || *source == '}' )
                    {
                    continue;
                    }

                // Parse next tuple
                if ( (source=setTupleFromText( source, i )) == 0 )
                    {
                    parseError = true;
                    break;
                    }

				// Consume the trailing delimiter if there is one
				else
					{ 
					if (*source == ',')
						{
						source++;
						}
					}
                } 

            // Check for trailing '}'
            if ( parseError == false && *(Cpl::Text::stripSpace( source )) == '}' )
                {
                msg.getPayload().m_success = true;
                }
            }
        }


    // Process any change notifications
    checkForNotifications();

    // Return the client's message
    msg.returnToSender();
    }


const char* Base::setTupleFromText( const char* source, unsigned tupleIdx )
    {
    // Trap: invalid starting notation (i.e. missing '(')
    source = Cpl::Text::stripSpace( source );
    if ( *source++ != '(' )
        {
        return 0;
        }


    // Loop through ALL elements
    Rte::Tuple::Api& tuple       = m_myPoint.getTuple(tupleIdx);
    bool             isContainer = m_myPoint.isContainer();
    unsigned         updated     = 0;
    unsigned         numElems    = tuple.getNumElements();
    unsigned         i;
    for(i=0; i<numElems && *source != '\0'; i++)
        {
        // Get element to update
        Rte::Element::Api& element = tuple.getElement(i);

        // Cache previous 'in-container' state (when a container tuple)
        Cpl::Text::FString<1> prevInContainer;
        if ( i == 0 && isContainer )
            {
            element.toString( prevInContainer );
            }

        // Update the element
        if ( (source = element.fromString( source, " ,)}", &updated)) )
            {
            // EXIT -->un-able to convert the string to a binary value (or bad syntax, i.e. failed to parse)
            return 0;
            }

        // Trap change 'in-container' state, aka membership changes
        if ( i == 0 && isContainer )
            {
            Cpl::Text::FString<1> newInContainer;
            if ( newInContainer != prevInContainer )
                {
                m_myPoint.incrementSequenceNumber();
                }
            }
        }


    // Mark the element/tuple as updated (i.e. support to using SeqNumbers for change detection)
    if ( updated )
        {
        tuple.setUpdatedState();
        tuple.incrementSequenceNumber();
        }

    // Everything is 'good' (in theory anyway)
    return source;
    }




