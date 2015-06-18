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

#include "RmwBase.h"

///
using namespace Rte::Point::Controller;


////////////////////////////////////////////
RmwBase::RmwBase( Rte::Point::Api&        readMemory, 
                  Rte::Point::Model::Api& modelPoint, 
                  RmwClient&              clientCallback 
                )
:m_myPoint(readMemory)
,m_modelPoint(modelPoint)
,m_clientCb(clientCallback)
    { 
    }


//////////////////////////////
void RmwBase::updateModel( void )
    {
    // Update the model
    m_modelPoint.update( m_myPoint, m_clientCb );
    }