#ifndef Rte_Element_Delta_h_
#define Rte_Element_Delta_h_
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

#include "Rte/Element/Basic.h"

///
namespace Rte { namespace Element {


/** This template class provides a specialized implementation of the
    Basic<numeric_type> element type such that the isDifferent() method is
    trigger on a delta change instead of simple not equal change. This element
    type is intended to be used ONLY with 'Viewers' of  the Model.  However, it
    will function the same as a Basic<numeric_type> element if contained in a
    Model point (or a Controller element), the negative is that it will consume
    more  RAM, i.e. storage for a the _delta member that is not used when an
    Model (or Controller) element.
 */
template<class ELEMTYPE, class PARENT>
class Delta: public PARENT
{
protected:
    /// cache the minimum delta required for a change
    ELEMTYPE    m_delta;


public:
    /// Constructor
    Delta( ELEMTYPE  delta        = 0,
           ELEMTYPE  initialValue = 0,
           bool      inUse        = false,
           int8_t    validState   = RTE_ELEMENT_API_STATE_INVALID
         );


public:
    /// Updates the element's delta setting 
    virtual void setDelta( ELEMTYPE newValue );

    /// Returns the element's delta setting
    virtual ELEMTYPE getDelta(void) const;


public: 
    /// See Rte::Element::Api
    bool isDifferentFrom( const Api& other ) const;
};

/////////////////////////////////////////////////////////////////////////////
// Pre-defined types to simply element declarations

/// C POD type
typedef Delta<int8_t,Integer8>       Integer8Delta_t;

/// C POD type
typedef Delta<uint8_t,Uinteger8>     Uinteger8Delta_t;

/// C POD type
typedef Delta<int16_t,Integer16>     Integer16Delta_t;

/// C POD type
typedef Delta<uint16_t,Uinteger16>   Uinteger16Delta_t;

/// C POD type
typedef Delta<int32_t,Integer32>     Integer32Delta_t;

/// C POD type
typedef Delta<uint32_t,Uinteger32>   Uinteger32Delta_t;

/// C POD type
typedef Delta<int64_t,Integer64>     Integer64Delta_t;

/// C POD type
typedef Delta<uint64_t,Uinteger64>   Uinteger64Delta_t;

/// C POD type
typedef Delta<size_t,SizeT>           SizeTDelta_T;

/// C POD type
typedef Delta<time_t,TimeT>           TimeTDelta_T;

/// C POD type
typedef Delta<float,Float>           FloatDelta_T;

/// C POD type
typedef Delta<double,Double>         DoubleDelta_T;

/// C POD type
typedef Delta<void*,VoidPtr>         VoidPtrDelta_T;


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class ELEMTYPE, class PARENT>
Rte::Element::Delta<ELEMTYPE,PARENT>::Delta( ELEMTYPE  delta,
                                             ELEMTYPE  initialValue,
                                             bool      inUse,
                                             int8_t    validState
                                           )
:PARENT(initialValue,inUse,validState),
 m_delta(delta)
    {
    }

/////////////////
template<class ELEMTYPE, class PARENT>
void Rte::Element::Delta<ELEMTYPE,PARENT>::setDelta( ELEMTYPE newDelta )
    {
    m_delta = newDelta;
    }

template<class ELEMTYPE, class PARENT>
ELEMTYPE Rte::Element::Delta<ELEMTYPE,PARENT>::getDelta( void ) const
    {
    return m_delta;
    }


/////////////////
template<class ELEMTYPE, class PARENT>
bool Rte::Element::Delta<ELEMTYPE,PARENT>::isDifferentFrom( const Api& other ) const
    {
    Base::assertTypeMatches( other );
    ELEMTYPE otherVal = *((ELEMTYPE*)(other.dataPointer()));
    ELEMTYPE delta    = PARENT::m_data > otherVal? PARENT::m_data - otherVal: otherVal - PARENT::m_data;
    return delta > m_delta;
    }


};      // end namespaces
};
#endif  // end header latch

