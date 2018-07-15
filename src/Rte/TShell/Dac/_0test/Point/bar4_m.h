#ifndef Point_ModelBar4_h_
#define Point_ModelBar4_h_
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
#include "Rte/TShell/Dac/_0test/Point/bar4.h"

///
namespace Point {


/** Concrete Model Point: BAR1
 */
class ModelBar4: public Bar4,
                 public Rte::Point::Model::Base
{
public:
    /// Constructor
    ModelBar4( Cpl::Itc::PostApi& myMbox )
        :Rte::Point::Model::Base(*this, myMbox)
            {
            }
};


};
#endif 