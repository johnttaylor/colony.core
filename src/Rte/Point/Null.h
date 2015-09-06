#ifndef Rte_Point_Null_h_
#define Rte_Point_Null_h_
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
/** @file */


#include "Rte/Point/Basic.h"
#include "Rte/Tuple/Null.h"


///
namespace Rte { namespace Point {


/** This template class implements a "Null Point" that contain N tuples.
    A Null Point is ONLY used/needed for the Lite Weight Viewer mechanism.
    The template parameter 'N' is the number of Null Tuples in the concrete 
    Point.
 */
template<int N>
class Null: public Rte::Point::Basic<N>
{
protected:
    /// Null Tuple instances
    Rte::Tuple::Null  m_nullTuples[N];

protected:
    /// Constructor
    Null( void );

};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<int N>
Rte::Point::Null<N>::Null( void )
    {
    unsigned i;
    for(i=0; i<N; i++)
        {
        registerTuple( i, m_nullTuples[i] );
        }
    }


};      // end namespaces
};
#endif  // end header latch
