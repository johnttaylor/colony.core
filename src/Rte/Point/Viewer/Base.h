#ifndef Rte_Point_Viewer_Base_h_
#define Rte_Point_Viewer_Base_h_
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


#include "Rte/Point/Viewer/Api.h"
#include "Rte/Point/Model/ViewerRequest.h"


///
namespace Rte { namespace Point { namespace Viewer {


/** This intermediate class provide common infrastructure for viewers
 */
class Base: public Api,
            public Rte::Point::Model::ViewerResponse
{
protected:
    /// Possible states
    enum State_t { eSTOPPED, eSTARTED, eSTOPPING };
    ///
    State_t                                             m_state;
    ///
    Rte::Point::Model::Api&                             m_modelPoint;
    ///
    Rte::Point::Model::ViewerRequest::RegisterPayload   m_registerPayload;
    ///
    Rte::Point::Model::ViewerResponse::RegisterMsg      m_registerMsg;
    ///
    Rte::Point::Model::ViewerRequest::CancelPayload     m_cancelPayload;
    ///
    Rte::Point::Model::ViewerResponse::CancelMsg        m_cancelMsg;


protected:
    /// Constructor
    Base( Rte::Point::Api&        myPoint, 
          Rte::Point::Model::Api& modelPoint,
          Cpl::Itc::PostApi&      viewerMbox
        );


public: 
    /// See Rte::Point::Viewer::Api
    unsigned startViewing( bool forceInitialUpdate = true, bool useValueForDifference = true );

    /// See Rte::Point::Viewer::Api
    bool stopViewing( void );

    /// See Rte::Point::Viewer::Api
    bool poll( void );

    /// See Rte::Point::Viewer::Api
    Rte::Point::Model::Api& getModelPoint(void);

    /// See Rte::Point::Viewer::Api
    Rte::Point::Api& getMyPoint(void);


public: 
    /// Rte::Point::Model::ViewerResponse
    void response( RegisterMsg& msg );
    
    /// Rte::Point::Model::ViewerResponse
    void response( CancelMsg& msg );


protected: // Helper methods
    ///
    virtual void modelHasChanged( void ) = 0;
    ///
    virtual void viewerHasStopped( void ) = 0;
};

};      // end namespaces
};
};
#endif  // end header latch
