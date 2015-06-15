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
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"


///
using namespace Rte::Point::Viewer; 

#define SECT_   "Rte::Point::Viewer"


/////////////////
Base::Base( Rte::Point::Api&        myPoint, 
            Rte::Point::Model::Api& modelPoint,
            Cpl::Itc::PostApi&      viewerMbox 
          )
:m_state( eSTOPPED )
,m_modelPoint( modelPoint )
,m_registerPayload( myPoint )
,m_registerMsg( *this, viewerMbox, modelPoint.getViewerSAP(), m_registerPayload )
,m_cancelPayload( m_registerMsg.getRequestMsg() )
,m_cancelMsg( *this, viewerMbox, modelPoint.getViewerSAP(), m_cancelPayload )
    {
    myPoint.invalidateAllTupleSequenceNumbers();
    }


/////////////////
unsigned Base::startViewing( bool forceInitialUpdate, bool useValueForDifference )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::startViewing - (%p)", this ));
    unsigned result = 0;

    // Ignore 'extra' start calls
    if ( m_state == eSTOPPED )
        {
        // Set my state to STARTED
        result  = 1;
        m_state = eSTARTED;

        // Set the viewer/client point to 'All Invalid' - this ensures that viewer/client
        // point will be updated with meaningful value(s) from the model point (or the 
        // default state of invalid is correct).
        m_registerPayload.m_viewerPoint.setAllValidFlagState(false);

        // Initialize my viewer to the current contents of the model point
        if ( forceInitialUpdate )
            {
            m_modelPoint.query( m_registerPayload.m_viewerPoint );  // Read the initial value
            m_registerPayload.m_viewerPoint.setMembershipChanged(); // Force membership change
            modelHasChanged();                                      // Generate callback
            }

        // Register with the model for future updates        
        m_registerPayload.m_compareValues = useValueForDifference;
        m_modelPoint.getViewerSAP().post( m_registerMsg.getRequestMsg() );  // Register for change notification
        }    

    return result;
    }

bool Base::poll( void )
    {
    bool result = true;

    // Force a change notification if I am current started/attached
    if ( m_state == eSTARTED )
        {
        m_modelPoint.pollViewer( m_registerMsg.getRequestMsg() ); 
        }

    // Issue a query if not started 
    else if ( m_state == eSTOPPED )
        {
        m_modelPoint.query( m_registerPayload.m_viewerPoint ); 
        modelHasChanged();          
        }
    
    // Fail the poll request because I don't own the 'm_viewerPoint' memory (it is is in transit)
    else
        {
        result = false;
        }

    return result;
    }

bool Base::stopViewing( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::stopViewing - (%p)", this ));

    // Ignore 'extra' stop calls
    if ( m_state == eSTARTED )
        {
        m_state = eSTOPPING;
        m_modelPoint.getViewerSAP().post( m_cancelMsg.getRequestMsg() );   // Send cancel notifications request
        }    


    return m_state == eSTOPPED;
    }


Rte::Point::Model::Api& Base::getModelPoint( void )
    {
    return m_modelPoint;
    }

Rte::Point::Api& Base::getMyPoint( void )
    {
    return m_registerPayload.m_viewerPoint;
    }


/////////////////
void Base::response( RegisterMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::response(RegisterMsg) - (%p)", this ));

    // Trap any usage error (i.e. the context/application programmer getting the start/stop wrong)
    if ( m_state == eSTOPPED )
        {
        Cpl::System::FatalError::logf( "Base::response::Register - State Protocol Error" );
        }
    else
        {
        // Notify context
        modelHasChanged();

        // Do not re-register if I am in the process of stopping
        if ( m_state == eSTARTED )
            {
            // Re-register for the next change notification
            m_modelPoint.getViewerSAP().post( m_registerMsg.getRequestMsg() );
            }
        }
    }
    
void Base::response( CancelMsg& msg )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Base::response(CancelMsg) - (%p)", this ));

    // Trap any usage error (i.e. the context/application programmer getting the start/stop wrong)
    if ( m_state != eSTOPPING )
        {
        Cpl::System::FatalError::logf( "Base::response::Cancel - State Protocol Error" );
        }
    else
        {
        m_state = eSTOPPED;
        viewerHasStopped();
        }
    }
