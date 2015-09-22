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
#include "Cpl/Text/atob.h"
#include "Cpl/Text/strip.h"



///
using namespace Rte::Element;

#define IN_USE_MASK_            0x01
#define LOCK_REQ_MASK_          0x02
#define UNLOCK_REQ_MASK_        0x04
#define REQUESTS_MASK_          (LOCK_REQ_MASK_|UNLOCK_REQ_MASK_)
#define IS_MODEL_ELEMENT_MASK_  0x08

#define LOCK_STATE_MASK_        0x80
#define VALID_MASK_             0x7F


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
    m_valid &= ~VALID_MASK_;
    m_valid |= newState & VALID_MASK_;
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
    return (m_valid & LOCK_STATE_MASK_) != 0;
    }

void Base::setLockedState( bool newState )
    {
    m_valid &= ~LOCK_STATE_MASK_;
    if ( newState )
        {    
        m_valid |= LOCK_STATE_MASK_;
        }
    else
        {
        m_valid &= ~LOCK_STATE_MASK_;
        }
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


////////////////////////
bool Base::isModelElement(void) const
    {
    return (m_opers & IS_MODEL_ELEMENT_MASK_) != 0;
    }

void Base::setAsModelElement_(void)
    {
    m_opers |= IS_MODEL_ELEMENT_MASK_;
    }




////////////////////////
bool Base::convertStateToText( Cpl::Text::String& dstMemory, bool& append ) const
    {
    // Indicate locked state
    if ( isLocked() )
        {
        dstMemory.formatOpt( append, "%c", OPTION_RTE_ELEMENT_LOCK_CHAR );
        append = true;
        }

    // All done if the element has a valid value
    int8_t status = validState();
    if ( status == RTE_ELEMENT_API_STATE_VALID )
        { 
        return true;
        }

    // Invalid -->default invalid state
    if ( status == RTE_ELEMENT_API_STATE_INVALID )
        {
        dstMemory.formatOpt( append, "%c", OPTION_RTE_ELEMENT_INVALID_CHAR );
        }

    // Invalid -->application specific value
    else
        {
        dstMemory.formatOpt( append, "%c%d", OPTION_RTE_ELEMENT_INVALID_CHAR, status );
        }

    return false;
    }


const char* Base::fromString( const char* srcText, const char* terminationChars, unsigned* updatedPtr )
    {
    // Trap prefix operation that apply to the Element 
    bool updated         = false;
    bool lockAction      = false;
    bool unlockAction    = false;
    bool invalidAction   = false;
    srcText              = parsePrefixOps( srcText, lockAction, unlockAction, invalidAction, updated, terminationChars );

    // NOT an 'invalidate' request
    if ( !invalidAction )
        {
        // Enforce LOCK semantics
        if ( !isLocked() )
            {
            // Set the element's value based on the source text
            srcText = setFromText( srcText, terminationChars );

            // By defintion a succesful update of an Element moves it to the Valid state.
            setValid();     
            updated = true;
            }

        // LOCK Operations MUST be applied AFTER any status/update operations
        if ( lockAction )
            {
            if ( !isLocked() )
                {
                setLocked();
                updated = true;
                }
            }
        else if ( unlockAction )
            {
            if ( isLocked() )
                {
                setUnlocked();
                updated = true;
                }
            }
        }

    // Return the remaining 'un-parsed' text (or ZERO if any error)
    if ( updated && updatedPtr )
        {
        *updatedPtr += 1;
        }
    return srcText;
    }


const char* Base::parsePrefixOps( const char* source, bool& lockAction, bool& unlockAction, bool& invalidAction, bool& updated, const char* terminationChars )
    {
    // Do nothing when there is NO 'source'
    if ( source == 0 || *source == '\0' )
        {
        return source;
        }

    // Remove leading whitespace
    source = Cpl::Text::stripSpace( source );

    // check for lock/unlock operations
    if ( *source == OPTION_RTE_ELEMENT_LOCK_CHAR )
        {
        lockAction = true;
        source++;
        }
    else if ( *source == OPTION_RTE_ELEMENT_UNLOCK_CHAR )
        {
        unlockAction = true;
        source++;
        }

    // Check for invalidate operation
    if ( *source == OPTION_RTE_ELEMENT_INVALID_CHAR )
        {
        const char* endPtr  = 0;
        int         invalid = RTE_ELEMENT_API_STATE_INVALID;
        if ( !Cpl::Text::a2i( invalid, source+1, 10, terminationChars, &endPtr ) || invalid < 1 || invalid > 127)
            {
            // Force the default invalid state value on error (or when missing numeric value)
            invalid = RTE_ELEMENT_API_STATE_INVALID;
            }

        // Apply invalidate - but ONLY when not LOCKED
        if ( !isLocked() )
            {
            invalidAction = true;
            setValidState( invalid );
            updated = true;
            }

        source = endPtr;
        }
  
    // Return the next 'un-parsed' character
    return source;
    }          

