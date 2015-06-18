#ifndef Rte_Point_Model_Api_h_
#define Rte_Point_Model_Api_h_
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

#include "Rte/Point/Model/ViewerRequest.h"
#include "Rte/Point/Model/ControllerRequest.h"
#include "Rte/Point/Model/QueryRequest.h"


///
namespace Rte { namespace Point { namespace Model {


/** This class defines the interface for the base operations of
    Model Point contained in the Model Model
 */
class Api
{
public:
    /** This method synchronously updates the Model Point
     */
    virtual void update( Rte::Point::Api& controllerPoint ) = 0;

    /** This method synchronously updates a single Tuple in the Model Point
     */
    virtual void update( Rte::Tuple::Api& controllerTuple, unsigned tupleIdx, bool membershipChanged = false ) = 0;

    /** This method synchronously Performs a Read-Modify-Write operation on the Model Point
     */
    virtual void update( Rte::Point::Api& rmwPoint, Rte::Point::Controller::RmwClient& modifyCallback ) = 0;

    /** This method synchronously Performs a Read-Modify-Write operation on a Model Container Point
     */
    virtual void update( Rte::Tuple::Api&                            rmwTuple,
                         bool                                        membershipChanged,
                         Rte::Point::Controller::RmwContainerClient& modifyCallback,
                         unsigned                                    itemIndexStart = 0   
                       ) = 0;


public:
    /** This method synchronously queries the Model Point
     */
    virtual void query( Rte::Point::Api& dstPoint, QueryRequest::Option_T copyOption = QueryRequest::eCOPY ) = 0;

    /** This method synchronously queries a single Tuple in the Model Point.
        The 'walkCallbackPtr' is an optional argument that ONLY applies when
        querying a Container Point.  The 'walkCallbackPtr' allows a client to
        walk/traverse/read every Tuple in the Container Point - one Tuple at a
        time (i.e. only need memory for a single Tuple). The traversal of the
        Tuples starts with Tuple index 'tupleIdx'.
     */
    virtual void query( Rte::Tuple::Api& dstTuple, unsigned tupleIdx, Rte::Point::Query::Traverser* walkCallbackPtr = 0, QueryRequest::Option_T copyOption = QueryRequest::eCOPY  ) = 0;
          

public:
    /** This method is used force a change notification to a viewer, aka Poll 
        a viewer.
     */
    virtual void pollViewer( ViewerRequest::RegisterMsg& viewerToPoll ) = 0;
    

public:
    /// Returns the SAP to the viewer ITC interface
    virtual Rte::Point::Model::ViewerRequest::SAP&      getViewerSAP(void) = 0;

    /// Returns the SAP to the controller ITC interface
    virtual Rte::Point::Model::ControllerRequest::SAP&  getControllerSAP(void) = 0;

    /// Returns the SAP to the query ITC interface
    virtual Rte::Point::Model::QueryRequest::SAP&       getQuerySAP(void) = 0;


public: 
    /// Virtual destructor to make the compiler happy
    virtual ~Api(void){}
};

};      // end namespaces
};
};
#endif  // end header latch