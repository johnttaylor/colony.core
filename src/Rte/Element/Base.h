#ifndef Rte_Element_Base_h_
#define Rte_Element_Base_h_
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

#include "Rte/Element/Api.h"


///
namespace Rte { namespace Element {


/** This partially concrete class provide common infrastructure for
    a element
 */
class Base: public Api
{
protected:
    /// My Type ID
    DataType    m_myDataType;

    /// My valid state AND current locked/unlocked state
    int8_t      m_valid;

    /// My in-use and lock/unlock requests
    uint8_t     m_opers;


protected:
    /// Constructor
    Base( int    myDataType,
          bool   inUse      = false,
          int8_t validState = RTE_ELEMENT_API_STATE_INVALID,
          bool   locked     = false
        );


protected:
    /// Throws a fatal error if there is Type mismatch
    virtual void assertTypeMatches( const Api& other ) const;


public:
    ///  See Rte::Element::Api
    const DataType getType(void) const;

    ///  See Rte::Element::Api
    bool isValid(void) const;

    ///  See Rte::Element::Api
    void setValidState( int8_t newState );

    ///  See Rte::Element::Api
    int8_t validState(void) const;

    ///  See Rte::Element::Api
    bool isInUse(void) const;

    ///  See Rte::Element::Api
    void setInUseState( bool newState );
    
    ///  See Rte::Element::Api
    bool isLocked(void) const;
    
    ///  See Rte::Element::Api
    void setLockedState( bool newState );

    ///  See Rte::Element::Api
    bool isLockRequest(void) const;
    
    ///  See Rte::Element::Api
    bool isUnlockRequest(void) const;

    ///  See Rte::Element::Api
    void requestLockOperation( bool newState );

    ///  See Rte::Element::Api
    void clearLockOperation(void);

    ///  See Rte::Element::Api
    void setRawValidState_( int8_t newRawState );

    ///  See Rte::Element::Api
    int8_t getRawValidState_(void) const;

    ///  See Rte::Element::Api
    bool isModelElement(void) const;

    ///  See Rte::Element::Api
    void setAsModelElement_(void);


};

};      // end namespaces
};
#endif  // end header latch
