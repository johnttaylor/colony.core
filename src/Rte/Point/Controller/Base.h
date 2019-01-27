#ifndef Rte_Point_Controller_Base_h_
#define Rte_Point_Controller_Base_h_
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

#include "Rte/Point/Controller/Api.h"
#include "Rte/Point/Model/Api.h"


///
namespace Rte { namespace Point { namespace Controller {


/** This concrete class implements the Controller Point interface 
 */
class Base: public Api
{
protected:
    /// Destination to where to write to data to
    Rte::Point::Api&         m_myPoint;

    /// The Model Point to read the data from (aka the source)
    Rte::Point::Model::Api&  m_modelPoint;


protected:
    /// Constructor
    Base( Rte::Point::Api& myPoint, Rte::Point::Model::Api& modelPoint );


public: 
    /// See Rte::Point::Controller::Api
    virtual bool updateModel( void );


};


};      // end namespaces
};
};
#endif  // end header latch
