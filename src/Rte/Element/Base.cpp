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

#include "Base.h"
#include "Cpl/System/FatalError.h"


///
using namespace Rte::Element;

#define IN_USE_MASK_        0x01
#define LOCK_REQ_MASK_      0x02
#define UNLOCK_REQ_MASK_    0x04
#define REQUESTS_MASK_      (LOCK_REQ_MASK_|UNLOCK_REQ_MASK_)

#define LOCK_STATE_MASK_    0x80
#define VALID_MASK_         (~LOCK_STATE_MASK_)


////////////////////////
Base::Base( int    myDataType,
            bool   inUse,
            int8_t validState,
            bool   locked
           )
:m_myDataType(myDataType)
,m_valid( validState & VALID_MASK_ )
,m_opers( inUse? IN_USE_MASK_: 0 )
    {
    m_valid |= locked? LOCK_STATE_MASK_: 0;
    }


////////////////////////
void Base::assertTypeMatches( const Api& other ) const
    {
    if ( m_myDataType.isNotEqual(other.getType()) )
        {
        Cpl::System::FatalError::logf( "Rte::Element::Base - type mismatch (me=%d, other=%d)", m_myDataType, other.getType() );
        }
    }


////////////////////////
const DataType Base::getType(void) const
    {
    return m_myDataType;
    }

bool Base::isValid(void) const
    {
    return (m_valid & VALID_MASK_) == RTE_ELEMENT_API_STATE_VALID;
    }

void Base::setValidState( int8_t newState )
    {
    m_valid = newState & VALID_MASK_;
    }


int8_t Base::validState(void) const
    {
    return m_valid & VALID_MASK_;
    }


////////////////////////
bool Base::isInUse(void) const
    {
    return m_opers & IN_USE_MASK_;
    }

void Base::setInUseState( bool newState )
    {
    if ( newState )
        {
        m_opers |= IN_USE_MASK_;
        }
    else
        {
        m_opers &= ~IN_USE_MASK_;
        }
    }


////////////////////////
bool Base::isLocked(void) const
    {
    return (m_valid & LOCK_STATE_MASK_) != 0;  // Note: the logical comparsion is need to avoid compiler warnings under Visual Studio
    }

void Base::setLocked(void)
    { 
    m_valid |= LOCK_STATE_MASK_;
    }

void Base::setUnlocked(void)
    {
    m_valid &= ~LOCK_STATE_MASK_;
    }


////////////////////////
bool Base::isLockRequest(void) const
    {
    return (m_opers & LOCK_REQ_MASK_) != 0;
    }

bool Base::isUnlockRequest(void) const
    {
    return (m_opers & UNLOCK_REQ_MASK_) != 0;
    }

void Base::requestLockOperation( bool newState )
    {
    m_opers &= ~REQUESTS_MASK_;
    m_opers |= newState? LOCK_REQ_MASK_: UNLOCK_REQ_MASK_;
    }

void Base::clearLockOperation(void)
    {
    m_opers &= ~REQUESTS_MASK_;
    }


////////////////////////
void Base::setRawValidState_( int8_t newRawState )
    {
    m_valid = newRawState;
    }


int8_t Base::getRawValidState_(void) const
    {
    return m_valid;
    }
