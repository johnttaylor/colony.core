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


#include "Null.h"
#include "Rte/Element/Basic.h"
#include "Cpl/System/FatalError.h"



///
using namespace Rte::Tuple;


/// Dummy Element
static Rte::Element::Boolean_T  dummy_;


/////////////////////
Null::Null( void )
:Base()
    {
    }


/////////////////
unsigned Null::getNumElements( void ) const
    {
    return 0;
    }


Rte::Element::Api& Null::getElement( unsigned elementIdx ) const
    {
    Cpl::System::FatalError::logf( "Rte::Tuple::Null::getElement - out-of-range index (maxIdx=n/a, requestIdx=%u)", elementIdx );

    // I WILL NEVER GET BECAUSE OF FATAL ERROR -->But I need to return something to keep the compiler happy!
    return dummy_;
    }

