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

#include "Base.h"

using namespace Rte::Point::Query;


///////////////////
Base::Base( Rte::Point::Api& myPoint, Rte::Point::Model::Api& modelPoint )
:m_myPoint(myPoint)
,m_modelPoint(modelPoint)
    {
    // Default to querying EVERYTHING
    m_myPoint.setAllInUseState(true);
    }


///////////////////
void Base::issueQuery( void )
    {
    // issue the query
    m_modelPoint.query(m_myPoint);
    }

