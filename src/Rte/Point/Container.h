#ifndef Rte_Point_Container_h_
#define Rte_Point_Container_h_
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


#include "Rte/Point/ContainerApi.h"
#include "Cpl/System/FatalError.h"


///
namespace Rte { namespace Point {




/** This template implements a Point that contains an array of SAME type
    tuples where the intent is that the Point is a container of dynamic
    number of Tuples.  

    Note: The Tuple type/class MUST inherit from the Rte::Tuple::ContainerItem class
 */
template<class TUPLE, int N>
class Container: public ContainerApi
{
public:
    /// Tuple instances 
    TUPLE m_tuples_[N];


protected:
    /// Constructor
    Container( void );


public:
    /// Provide public access to my tuples
    inline TUPLE& operator[] (unsigned i) { return m_tuples_[i]; }


public: 
    /// See Rte::Point::Api
    unsigned getNumTuples(void) const;

    /// See Rte::Point::Api
    Rte::Tuple::Api& getTuple( unsigned tupleIdx );


public: 
    /// See Rte::Point::ContainerApi
    void addTuple( unsigned tupleIdx );

    /// See Rte::Point::ContainerApi
    void removeTuple( unsigned tupleIdx );

    /// See Rte::Point::ContainerApi
    bool isTupleInContainer( unsigned tupleIdx ) const;

    /// See Rte::Point::ContainerApi
    int nextTuple( unsigned startIdx ) const;

    /// See Rte::Point::ContainerApi
    int nextEmptyTuple( unsigned startIdx ) const;
};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class TUPLE, int N>
Rte::Point::Container<TUPLE,N>::Container( void )
    {
    // Initialize all of the tuples to: NOT in container
    unsigned i;;
    for(i=0; i<N; i++)
        {
        m_tuples_[i].m_inContainer_.set(false);
        }
    }

/////////////////
template<class TUPLE, int N>
unsigned Rte::Point::Container<TUPLE,N>::getNumTuples( void ) const
    {
    return N;
    }

template<class TUPLE, int N>
Rte::Tuple::Api& Rte::Point::Container<TUPLE,N>::getTuple( unsigned tupleIdx ) 
    {
    if ( tupleIdx >= N )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Container::getTuple - out-of-range index (maxIdx=%u, requestIdx=%u)", N, tupleIdx );
        }

    return m_tuples_[tupleIdx];
    }

/////////////////
template<class TUPLE, int N>
void Rte::Point::Container<TUPLE,N>::addTuple( unsigned tupleIdx  )
    {
    if ( tupleIdx >= N )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Container::addTuple - out-of-range index (maxIdx=%u, requestIdx=%u)", N, tupleIdx );
        }

    m_tuples_[tupleIdx].m_inContainer_.set(true);
    m_tuples_[tupleIdx].m_inContainer_.setValid();
    m_tuples_[tupleIdx].m_inContainer_.setInUse();
    }

template<class TUPLE, int N>
void Rte::Point::Container<TUPLE,N>::removeTuple( unsigned tupleIdx  )
    {
    if ( tupleIdx >= N )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Container::removeTuple - out-of-range index (maxIdx=%u, requestIdx=%u)", N, tupleIdx );
        }

    m_tuples_[tupleIdx].m_inContainer_.set(false);
    m_tuples_[tupleIdx].m_inContainer_.setValid();
    m_tuples_[tupleIdx].m_inContainer_.setInUse();
    }

template<class TUPLE, int N>
bool Rte::Point::Container<TUPLE,N>::isTupleInContainer( unsigned tupleIdx ) const
    {
    if ( tupleIdx >= N )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Container::isTupleInContainer - out-of-range index (maxIdx=%u, requestIdx=%u)", N, tupleIdx );
        }

    return m_tuples_[tupleIdx].m_inContainer_.get() == true;
    }

template<class TUPLE, int N>
int Rte::Point::Container<TUPLE,N>::nextTuple( unsigned startIdx ) const
    {
    while( startIdx < N )
        {
        if ( m_tuples_[startIdx].m_inContainer_.get() == true )
            {
            return startIdx;
            }

        startIdx++;
        }

    return -1;
    }

template<class TUPLE, int N>
int Rte::Point::Container<TUPLE,N>::nextEmptyTuple( unsigned startIdx ) const
    {
    while( startIdx < N )
        {
        if ( m_tuples_[startIdx].m_inContainer_.get() == false )
            {
            return startIdx;
            }

        startIdx++;
        }

    return -1;
    }

};      // end namespaces
};
#endif  // end header latch
