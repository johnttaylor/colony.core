#ifndef Rte_Tuple_Null_h_
#define Rte_Tuple_Null_h_
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


#include "Rte/Tuple/Base.h"



///
namespace Rte { namespace Tuple {


/** This class implements a Null Tuple.  A Null Tuple is a Tuple with zero
    Elements.  A Null Tuple is typically only used to implement Light Weight
    Viewers. 
 */
class Null: public Base
{
public:
    /// Constructor. 
    Null( void );


public: 
    /// See Rte::Tuple::Api
    unsigned getNumElements(void) const;

    /// See Rte::Tuple::Api
    Rte::Element::Api& getElement( unsigned elementIdx ) const;


};



};      // end namespaces
};
#endif  // end header latch
