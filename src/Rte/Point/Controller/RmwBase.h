#ifndef Rte_Point_Controller_RmwBase_h_
#define Rte_Point_Controller_RmwBase_h_
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

#include "Rte/Point/Controller/Api.h"
#include "Rte/Point/Controller/RmwClient.h"
#include "Rte/Point/Model/Api.h"

///
namespace Rte { namespace Point { namespace Controller {

/** This concrete provides the interface for atomically updating a
    non-container Point
 */
class RmwBase: public Api
{
protected:
    /// Reference to the client's Point that holds the result of the read 
    Rte::Point::Api&            m_myPoint;

    /// Reference to the Model point being modified
    Rte::Point::Model::Api&     m_modelPoint;

    /// Client to callback to perform the modify operation
    RmwClient&                  m_clientCb;

protected:
    ///
    RmwBase( Rte::Point::Api&        readMemory, 
             Rte::Point::Model::Api& modelPoint, 
             RmwClient&              clientCallback 
           );

public:  
    /// See Rte::Point::Controller::Api
    void updateModel( void );

};



};      // end namespaces
};
};
#endif  // end header latch
