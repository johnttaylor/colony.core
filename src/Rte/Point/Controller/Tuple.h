#ifndef Rte_Point_Controller_Tuple_h_
#define Rte_Point_Controller_Tuple_h_
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
#include "Rte/Tuple/Api.h"
#include "Rte/Point/Model/Api.h"

///
namespace Rte { namespace Point { namespace Controller {

/** This concrete class implements the Controller Point interface 
    for a Point that updates a SINGLE Tuple within the Point.
 */
class Tuple: public Api
{
protected:
    /// The client's Tuple data, aka the source Tuple for the write operation
    Rte::Tuple::Api&            m_myTuple;

    /// Refrence to the Model Point to be updated (aka the destination)
    Rte::Point::Model::Api&     m_modelPoint;

    /// Index of Tuple within the Model Point
    unsigned                    m_tupleIdx;


protected:
    /// Constructor
    Tuple( unsigned tupleIndex, Rte::Tuple::Api& myTuple, Rte::Point::Model::Api& modelPoint );


public: 
    /// See Rte::Point::Controller::Api
    bool updateModel( void );
};


};      // end namespaces
};
};
#endif  // end header latch
