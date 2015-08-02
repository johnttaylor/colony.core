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
using namespace Rte::Point::Query;


///////////////////////////////////////////////////
Tuple::Tuple( unsigned                                  tupleIndex, 
              Rte::Tuple::Api&                          myTuple, 
              Rte::Point::Model::Api&                   modelPoint, 
              Rte::Point::Model::QueryRequest::Option_T copyOption
            )
:m_myTuple(myTuple)
,m_modelPoint(modelPoint)
,m_tupleIdx(tupleIndex)
,m_copyOption(copyOption)
    {
    }


///////////////////////////////////////////////////
void Tuple::issueQuery( void )
    {
    m_modelPoint.query( m_myTuple, m_tupleIdx, 0, m_copyOption );
    }


void Tuple::setTupleIndex( unsigned newTupleIndex )
    {
    m_tupleIdx = newTupleIndex;
    }
