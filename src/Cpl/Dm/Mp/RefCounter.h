#ifndef Cpl_Rte_Mp_RefCounter_h_
#define Cpl_Rte_Mp_RefCounter_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Dm/Mp/Basic.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is 32
    bit unsigned reference counter.  A reference counter can be increment or
    decremented.  When incremented, the value is clamped at 2^16 (not allowed
    to overflow).  When decremented, the value is clamped at zero (not allowed 
    to underflow).  In addition, change notifications are only generated on the 
    following transitions:

        1) Invalid state to the Valid state
        2) Valid state to the Invalid state
        3) Transition to zero 
        4) Transition from zero to value greater than zero
    
    For the fromString() operation.  The provide value is used to either
    increment, decrement, and reset the reference counter.  For example:
    
        The following strings increment the counter by 1,3,14 respectively:
            "+1", "!+3", "^+14"

        The following strings decrement the counter by 1,3,14 respectively:
            "!-1", "-3", "^-4"  

        The following strings reset the counter to 0,1,14 respectively:
            "^0", "!1", "14"


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class RefCounter : public Basic<uint32_t>
{
public:
    /// Constructor. Invalid MP
    RefCounter( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo  );

    /// Constructor. Valid MP.  Requires an initial value
    RefCounter( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, uint32_t initialValue  );

public:
    /**  See Cpl::Dm::ModelPoint.
        Note: When the Model Point is invalidate, the internal counter is reset 
              to zero, i.e. calling increment() when the Model Point is in the
              invalid state will result the counter being set to 1.
     */
    uint16_t setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Reads the current counter value
    virtual int8_t read( uint32_t& dstData, uint16_t* seqNumPtr=0 ) const noexcept;

    /// Increments the counter
    virtual uint16_t increment( uint32_t incrementAmount=1, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Decrements the counter
    virtual uint16_t decrement( uint32_t decrementAmount=1, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;
 
    /// Resets the counter to zero (or to a specific value)
    virtual uint16_t reset( uint32_t newValue=0, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<RefCounter> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) noexcept;


public:
    ///  See Cpl::Dm::ModelPoint.
    bool toString( Cpl::Text::String& dst, bool append=false, uint16_t* retSequenceNumber=0 ) const noexcept;

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;
};



};      // end namespaces
};
};
#endif  // end header latch
