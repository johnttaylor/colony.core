#ifndef Point_ModelBar1_h_
#define Point_ModelBar1_h_
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
/** @file */

#include "Rte/Point/Model/Base.h"
#include "Rte/Point/Model/_0test/Point/bar1.h"

///
namespace Point {


/** Concrete Model Point: BAR1
 */
class ModelBar1: public Bar1,
                 public Rte::Point::Model::Base
{
public:
    /// Constructor
    ModelBar1( Cpl::Itc::PostApi& myMbox )
        :Rte::Point::Model::Base(*this, myMbox)
            {
            }

    /// Default values
    void defaultMe(void)
        {
        m_myPoint.setAllValidState( RTE_ELEMENT_API_STATE_VALID );
        m_tuple.m_name.set( "default" );
        m_tuple.m_enabled.set( false );
        m_tuple.m_count.set( 666 );
        }
};


};
#endif 