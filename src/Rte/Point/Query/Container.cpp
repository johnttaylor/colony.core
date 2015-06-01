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
using namespace Rte::Point::Query;


///////////////////
Container::Container( Rte::Point::ContainerApi& myPoint, Rte::Point::Model::Api& modelPoint )
:m_myPoint(myPoint)
,m_modelPoint(modelPoint)
    {
    }


///////////////////
void Container::issueQuery( void )
    {
    // Set the query/client point to 'All Invalid' - this ensures that query/client
    // point will be updated with meaningful value(s) from the model point (or the 
    // default state of invalid is correct).
    m_myPoint.setAllValidFlagState( false );

    // issue the query
    m_modelPoint.query( m_myPoint );
    }

