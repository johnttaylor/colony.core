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

#include "Container.h"


///
using namespace Rte::Point::Controller;

///////////////////
Container::Container( Rte::Point::ContainerApi& myPoint, Rte::Point::Model::Api& modelPoint )
:m_myPoint(myPoint)
,m_modelPoint(modelPoint)
    {
    }


///////////////////
void Container::updateModel( void  )
    {
    m_modelPoint.update( m_myPoint );

    // Ensure that the sequence number gets reset if the client reuses the controller 
    m_myPoint.invalidateSequenceNumber();
    }

