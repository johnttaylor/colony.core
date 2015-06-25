#ifndef Rte_Point_Basic_h_
#define Rte_Point_Basic_h_
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


#include "Rte/Point/Base.h"


///
namespace Rte { namespace Point {


/** This template class provides the infrastructure managing the N Tuples
    in a Point (and managing them as abstract Tuple Type). The template 
    parameter 'N' is the number of Tuples in the concrete Point.
 */
template<int N>
class Basic: public Rte::Point::Base
{
protected:
    /// Pointers to all of my contained Tuples
    Rte::Tuple::Api*  m_tuplePtrs_[N];

protected:
    /// Constructor
    Basic( void );


protected:
    /** This method attaches/registers the Tuple into the array of Tuples.
        This method should ONLY be called in the constructor of the concrete
        child class. 
     */
    virtual void registerTuple( unsigned tupleIdx, Rte::Tuple::Api& tuple );



public: 
    /// See Rte::Point::Api
    unsigned getNumTuples(void) const;

    /// See Rte::Point::Api
    Rte::Tuple::Api& getTuple( unsigned tupleIdx );
};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<int N>
Rte::Point::Basic<N>::Basic( void )
    {
    // Initialize tuple array (for error trapping)
    unsigned i;
    for(i=0; i<N; i++)
        {
        m_tuplePtrs_[i] = 0;
        }
    }

/////////////////
template<int N>
void Rte::Point::Basic<N>::registerTuple( unsigned tupleIdx, Rte::Tuple::Api& tuple )
    {
    if ( tupleIdx >= N )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Basic::registerTuple - exceeded max number of tuples (N=%u)", N );
        }

    m_tuplePtrs_[tupleIdx] = &tuple;
    }


/////////////////
template<int N>
unsigned Rte::Point::Basic<N>::getNumTuples( void ) const
    {
    return N;
    }

template<int N>
Rte::Tuple::Api& Rte::Point::Basic<N>::getTuple( unsigned tupleIdx ) 
    {
    if ( tupleIdx >= N )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Basic::getTuple - out-of-range index (maxIdx=%u, requestIdx=%u)", N, tupleIdx );
        }

    Rte::Tuple::Api* ptr = m_tuplePtrs_[tupleIdx];
    if ( !ptr )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Basic::getTuple - un-initialize tuple index (%u)", tupleIdx );
        }

    return *ptr;
    }


};      // end namespaces
};
#endif  // end header latch
