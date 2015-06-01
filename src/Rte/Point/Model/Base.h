#ifndef Rte_Point_Model_Base_h_
#define Rte_Point_Model_Base_h_
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

#include "Rte/Point/Model/Api.h"
#include "Cpl/Container/SList.h"
#include "Cpl/Itc/PostApi.h"


///
namespace Rte { namespace Point { namespace Model {

/** This concrete class handles the ITC processing for a Model Point,
    i.e. generates change notifications to viewers and accepts updates
    from controllers.
 */
class Base: public Api,
            //public Rte::Point::Model::ViewerRequest,
            public Rte::Point::Model::ControllerRequest,
            public Rte::Point::Model::QueryRequest
{
protected:
    /// The actual Point data
    Rte::Point::Api&                            m_myPoint;

    /// List of Viewers waiting on changes
//    Cpl::Container::SList<AttachMsg>            m_pendingNotify;

    /// SAP for my Viewer ITC interface
//    Rte::Point::Model::ViewerRequest::SAP       m_viewerSAP;

    /// SAP for my Controller ITC interface
    Rte::Point::Model::ControllerRequest::SAP   m_controllerSAP;

    /// SAP for my Query ITC interface
    Rte::Point::Model::QueryRequest::SAP        m_querySAP;


protected:
    ///
    Base( Rte::Point::Api& myPoint, Cpl::Itc::PostApi& myMbox );


public:
    /// See Rte::Point::Model::Api
    void update( Rte::Point::Api& controllerPoint );

    /// See Rte::Point::Model::Api
    void update( Rte::Tuple::Api& controllerTuple, unsigned tupleIdx );


public:
    /// See Rte::Point::Model::Api
    void query( Rte::Point::Api& dstPoint );

    /// See Rte::Point::Model::Api
    void query( Rte::Tuple::Api& dstTuple, unsigned tupleIdx );


public:
    /// See Rte::Point::Model::Api
//    void request( AttachMsg& msg );
//
//    /// See Rte::Point::Model::Api
//    void request( DetachMsg& msg );
//
//    /// See Rte::Point::Model::Api
//    void request( ForceMsg& msg );


public:
    /// See Rte::Point::Model::Api
    void request( UpdateMsg& msg );

    /// See Rte::Point::Model::Api
    void request( UpdateTupleMsg& msg ); 


public:
    /// See Rte::Point::Model::Api
    void request( QueryMsg& msg );

    /// See Rte::Point::Model::Api
    void request( QueryTupleMsg& msg );


public: 
    /// See Rte::Point::Model::Api
//    Rte::Point::Model::ViewerRequest::SAP&     getViewerSAP(void);

    /// See Rte::Point::Model::Api
    Rte::Point::Model::ControllerRequest::SAP& getControllerSAP(void);

    /// See Rte::Point::Model::Api
    Rte::Point::Model::QueryRequest::SAP&      getQuerySAP(void);


protected:
    /// Helper
    void checkForNotifications(void);

};


};      // end namespaces
};
};
#endif  // end header latch
