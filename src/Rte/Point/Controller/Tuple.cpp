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


#include "Tuple.h"

///
using namespace Rte::Point::Controller;


///////////////////////////////////////////////////
Tuple::Tuple( unsigned tupleIndex, Rte::Tuple::Api& myTuple, Rte::Point::Model::Api& modelPoint )
:m_myTuple(myTuple)
,m_modelPoint(modelPoint)
,m_tupleIdx(tupleIndex)
    {
    }


///////////////////////////////////////////////////
bool Tuple::updateModel( void )
    {
    m_modelPoint.update( m_myTuple, m_tupleIdx );
    return true;
    }
