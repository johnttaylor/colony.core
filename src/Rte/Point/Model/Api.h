#ifndef Rte_Point_Model_Api_h_
#define Rte_Point_Model_Api_h_
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
/** @file */

#include "Rte/Point/Model/ViewerRequest.h"
#include "Rte/Point/Model/ControllerRequest.h"
#include "Rte/Point/Model/QueryRequest.h"
#include "Rte/Point/Model/DefaultRequest.h"


///
namespace Rte { namespace Point { namespace Model {


/** This class defines the interface for the base operations of
    Model Point contained in the Model 
 */
class Api
{
public:
    /** This method updates cause the Model Point's data to be changed/updated
        to its 'default values' as defined by the application. 
         
        NOTE: This method is never called directly by the RTE engine, i.e. it 
              is the Application responsiblity to call this method (if at all).  
              There is one EXCEPTION to this rule - it is when the Rte::Db engine 
              initializes Records from non-volatile storage.  Each Record has an 
              associated Model Point and as part of the Db start-up process this 
              method is called on all of the associated Model Points.
    */
    virtual void defaultContents( void ) throw() = 0;

    /** This is non-thread safe version (i.e. can only be called from the 
        'Model' thread) of defaultContents().  Typically the Application 
        SHOULD NEVER call this method.
     */
    virtual void defaultContents_nonThreadSafe( void ) throw() = 0;


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
                         Rte::Point::Controller::RmwContainerClient& modifyCallback,
                         unsigned                                    itemIndexStart = 0   
                       ) = 0;

    /** This method synchronously updates a Model Point (or individual Tuple) 
        from a generic ASCII text string.  When 'tupleIdx' is less than zero 
        than the entire Point is updated; else only the specified tuple is 
        updated. The method returns true if succesful, else false is returned.
 
        NOTES: 

            o Parsing of the 'sourceValues' is destructive, i.e. the orignal
              content of 'sourceValues' is MODIFIED.
            o When false is returned the state of the Model Point IS NOT 
              GUARANTEED!
     */
    virtual bool update( Cpl::Text::String& sourceValues, int tupleIdx = -1 ) = 0;        


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
  
    /** This method synchronously queries a Model Point (or individual Tuple) and
        returns the result a generic ASCII text.  When 'tupleIdx' is less than
        zero than the entire Point is queried; else only the specified tuple
        is queried. The method returns true if succesful, else false is returned.
     */
    virtual bool query( Cpl::Text::String& results, int tupleIdx = -1 ) = 0;        


public:
    /** This method is used force a change notification to a viewer, aka Poll 
        a viewer.
     */
    virtual void pollViewer( ViewerRequest::RegisterMsg& viewerToPoll ) = 0;
    

public:
    /// Returns the SAP to the default ITC interface
    virtual Rte::Point::Model::DefaultRequest::SAP&     getDefaultSAP(void) = 0;

    /// Returns the SAP to the viewer ITC interface
    virtual Rte::Point::Model::ViewerRequest::SAP&      getViewerSAP(void) = 0;

    /// Returns the SAP to the controller ITC interface
    virtual Rte::Point::Model::ControllerRequest::SAP&  getControllerSAP(void) = 0;

    /// Returns the SAP to the query ITC interface
    virtual Rte::Point::Model::QueryRequest::SAP&       getQuerySAP(void) = 0;


public:
    /** This method returns the Model Point's internal Point.  This method is 
        typically ONLY used internally by the Rte Engine and should NOT be 
        called by the Application. This method is non-thread safe, i.e. 
        can ONLY be called from the 'Model' thread. 
     */
    virtual Rte::Point::Api& getMyPoint_nonThreadSafe(void) throw() = 0;
    
    /** This method 'touches' and/or marks the Model Point's internal Point
        as have been updated/modified.  This method is typically ONLY used 
        internally by the Rte Engine and should NOT be called by the 
        Application.. This method is non-thread safe, i.e. can ONLY be called 
        from the 'Model' thread.
     */
    virtual void touch_nonThreadSafe( void ) throw() = 0;



public: 
    /// Virtual destructor to make the compiler happy
    virtual ~Api(void){}
};

};      // end namespaces
};
};
#endif  // end header latch
