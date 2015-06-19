#ifndef Rte_Element_Delta_h_
#define Rte_Element_Delta_h_
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
template<class ELEMTYPE, int TYPEID>
class Delta: public Basic<ELEMTYPE,TYPEID>
{
protected:
    ///
    ELEMTYPE    m_delta;


public:
    /// Constructor
    Delta( ELEMTYPE  delta        = 0,
           ELEMTYPE  initialValue = 0,
           bool      inUse        = false,
           bool      validFlag    = false
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
typedef Delta<int8_t,DataType::INTEGER8>       Integer8Delta_t;

/// C POD type
typedef Delta<uint8_t,DataType::UINTEGER8>     Uinteger8Delta_t;

/// C POD type
typedef Delta<int16_t,DataType::INTEGER16>     Integer16Delta_t;

/// C POD type
typedef Delta<uint16_t,DataType::UINTEGER16>   Uinteger16Delta_t;

/// C POD type
typedef Delta<int32_t,DataType::INTEGER32>     Integer32Delta_t;

/// C POD type
typedef Delta<uint32_t,DataType::UINTEGER32>   Uinteger32Delta_t;

/// C POD type
typedef Delta<int64_t,DataType::INTEGER64>     Integer64Delta_t;

/// C POD type
typedef Delta<uint64_t,DataType::UINTEGER64>   Uinteger64Delta_t;

/// C POD type
typedef Delta<uint64_t,DataType::SIZE_T>       SizeTDelta_T;

/// C POD type
typedef Delta<float,DataType::FLOAT>           FloatDelta_t;

/// C POD type
typedef Delta<double,DataType::DOUBLE>         DoubleDelta_t;

/// C POD type
typedef Delta<void*,DataType::VOIDPTR>         VoidPtrDelta_t;


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class ELEMTYPE, int TYPEID>
Rte::Element::Delta<ELEMTYPE,TYPEID>::Delta( ELEMTYPE  delta,
                                             ELEMTYPE  initialValue,
                                             bool      inUse,
                                             bool      validFlag
                                           )
:Basic<ELEMTYPE,TYPEID>(initialValue,inUse,validFlag),
 m_delta(delta)
    {
    }

/////////////////
template<class ELEMTYPE, int TYPEID>
void Rte::Element::Delta<ELEMTYPE,TYPEID>::setDelta( ELEMTYPE newDelta )
    {
    m_delta = newDelta;
    }

template<class ELEMTYPE, int TYPEID>
ELEMTYPE Rte::Element::Delta<ELEMTYPE,TYPEID>::getDelta( void ) const
    {
    return m_delta;
    }


/////////////////
template<class ELEMTYPE, int TYPEID>
bool Rte::Element::Delta<ELEMTYPE,TYPEID>::isDifferentFrom( const Api& other ) const
    {
    Base::assertTypeMatches( other );
    ELEMTYPE otherVal = *((ELEMTYPE*)(other.dataPointer()));
    ELEMTYPE delta    = Basic<ELEMTYPE,TYPEID>::_data > otherVal? Basic<ELEMTYPE,TYPEID>::_data - otherVal: otherVal -Basic<ELEMTYPE,TYPEID>::_data;
    return delta > m_delta;
    }


};      // end namespaces
};
#endif  // end header latch

