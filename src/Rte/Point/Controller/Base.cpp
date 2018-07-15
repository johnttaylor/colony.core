/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Base.h"

using namespace Rte::Point::Controller;


///////////////////
Base::Base( Rte::Point::Api& myPoint, Rte::Point::Model::Api& modelPoint )
:m_myPoint(myPoint),
 m_modelPoint(modelPoint)
    {
    }


///////////////////
bool Base::updateModel( void )
    {
    m_modelPoint.update( m_myPoint );

    // Ensure that the sequence number gets reset if the client
    // reuses the controller 
    m_myPoint.resetSequenceNumber();
    return true;
    }

