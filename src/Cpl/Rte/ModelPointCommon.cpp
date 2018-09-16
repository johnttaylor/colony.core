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

#include "ModelPointCommon.h"
#include "MailboxServer.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/atob.h"

///
using namespace Cpl::Rte;

/// SubscriberApi States
enum State_T
{
    eSTATE_UNSUBSCRIBED = 0,          // Not subscribed to any model point
    eSTATE_IDLE,                      // Subscribed and waiting for a change
    eSTATE_NOTIFY_PENDING,            // Subscribed and waiting for next change notification dispatch cycle
    eSTATE_NOTIFY_NOTIFYING,          // The Client change notification callback is being executed
    eSTATE_NOTIFY_PENDING_DETACH,     // The subscription was requested to be cancelled during the change notification callback 
};


////////////////////////
ModelPointCommon::ModelPointCommon( ModelDatabaseApi& myModelBase, void* myDataPtr, StaticInfo& staticInfo, int8_t validState )
    : m_staticInfo( staticInfo )
    , m_modelDatabase( myModelBase )
    , m_dataPtr( myDataPtr )
    , m_seqNum( SEQUENCE_NUMBER_UNKNOWN + 1 )
    , m_locked( false )
    , m_validState( validState )
{
}

/////////////////
const char* ModelPointCommon::getName() const throw()
{
    return m_staticInfo.getName();
}

uint16_t ModelPointCommon::getSequenceNumber() const throw()
{
    m_modelDatabase.lock_();
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}

int8_t ModelPointCommon::getValidState( void ) const throw()
{
    m_modelDatabase.lock_();
    int8_t result = m_validState;
    m_modelDatabase.unlock_();
    return result;
}

uint16_t ModelPointCommon::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) throw()
{
    // Force a 'valid Invalid State value
    if ( newInvalidState <= 0 )
    {
        newInvalidState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID;
    }

    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        if ( IS_VALID( m_validState ) )
        {
            m_validState = newInvalidState;
            processChangeNotifications();
        }

        // Note: Update my state even if there was NO valid-->invalid transition - since there are many 'invalid states'
        else
        {
            m_validState = newInvalidState;
        }
    }

    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}

uint16_t ModelPointCommon::read( void* dstData, size_t dstSize, int8_t& validState ) const throw()
{
    m_modelDatabase.lock_();
    validState = m_validState;
    if ( IS_VALID( validState ) )
    {
        copyDataTo_( dstData, dstSize );
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t ModelPointCommon::write( const void* srcData, size_t srcSize, LockRequest_T lockRequest ) throw()
{
    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        if ( !IS_VALID( m_validState ) || isDataEqual_( srcData ) == false )
        {
            copyDataFrom_( srcData, srcSize );
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t ModelPointCommon::readModifyWrite( GenericRmwCallback& callbackClient, LockRequest_T lockRequest )
{
    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Invoke the client's callback function
        RmwCallbackResult_T result = callbackClient.genericCallback( m_dataPtr, m_validState );

        // Do nothing if the callback did not change anything
        if ( result != RmwCallbackResult_T::eNO_CHANGE )
        {
            // Handle request to invalidate the MP data
            if ( result == RmwCallbackResult_T::eINVALIDATE )
            {
                if ( IS_VALID( m_validState ) )
                {
                    m_validState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID;
                    processChangeNotifications();
                }
            }

            // Handle the CHANGED use case
            else
            {
                processDataUpdated();
            }
        }
    }

    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t ModelPointCommon::touch() throw()
{
    m_modelDatabase.lock_();
    processChangeNotifications();
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}


/////////////////
uint16_t ModelPointCommon::setLockState( LockRequest_T lockRequest ) throw()
{
    m_modelDatabase.lock_();
    if ( lockRequest == eLOCK )
    {
        m_locked = true;
    }
    else if ( lockRequest == eUNLOCK )
    {
        m_locked = false;
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}

bool ModelPointCommon::isLocked() const throw()
{
    m_modelDatabase.lock_();
    bool result = m_locked;
    m_modelDatabase.unlock_();
    return result;
}


/////////////////
size_t ModelPointCommon::exportData( void* dstDataStream, size_t maxDstLength, uint16_t* retSeqNum ) const throw()
{
    size_t result = 0;
    if ( dstDataStream )
    {
        m_modelDatabase.lock_();

        // Do nothing if there is not enough space left in the destination stream
        if ( maxDstLength >= getExternalSize() )
        {
            // Export Data
            size_t dataSize = getSize();
            memcpy( dstDataStream, getDataPointer_(), dataSize );

            // Export Valid State
            uint8_t* ptr = (uint8_t*) dstDataStream;
            memcpy( ptr + dataSize, &m_validState, sizeof( m_validState ) );

            // Return the Sequence number when requested
            if ( retSeqNum )
            {
                *retSeqNum = m_seqNum;
            }

            result = getExternalSize();
        }

        m_modelDatabase.unlock_();
    }
    return result;
}

size_t ModelPointCommon::importData( const void* srcDataStream, size_t srcLength, uint16_t* retSeqNum ) throw()
{
    size_t result = 0;
    if ( srcDataStream )
    {
        m_modelDatabase.lock_();

        // Fail the import when there is not enough data left in the input stream
        size_t dataSize = getSize();
        if ( dataSize <= srcLength )
        {
            // Import Data
            memcpy( getDataPointer_(), srcDataStream, dataSize );

            // Import Valid State
            uint8_t* ptr = (uint8_t*) srcDataStream;
            memcpy( &m_validState, ptr + dataSize, sizeof( m_validState ) );

            // Generate change notifications and return the Sequence number when requested
            processDataUpdated();
            if ( retSeqNum )
            {
                *retSeqNum = m_seqNum;
            }

            result = getExternalSize();
        }

        m_modelDatabase.unlock_();
    }
    return result;
}

size_t ModelPointCommon::getExternalSize() const throw()
{
    return getSize() + sizeof( m_validState );
}


/////////////////
int ModelPointCommon::compareKey( const Key& key ) const
{
    return m_staticInfo.compareKey( key );
}

const void* ModelPointCommon::getRawKey( unsigned* returnRawKeyLenPtr ) const
{
    return m_staticInfo.getRawKey( returnRawKeyLenPtr );
}

const Cpl::Container::Key& ModelPointCommon::getKey() const throw()
{
    return m_staticInfo;
}



/////////////////
void ModelPointCommon::processDataUpdated() throw()
{
    // By definition - Point now has valid date
    m_validState = MODEL_POINT_STATE_VALID;
    processChangeNotifications();
}

void ModelPointCommon::processChangeNotifications() throw()
{
    // Increment my sequence number (when rolling over -->do not allow the 'unknown' value)
    m_seqNum++;
    if ( m_seqNum == SEQUENCE_NUMBER_UNKNOWN )
    {
        m_seqNum = SEQUENCE_NUMBER_UNKNOWN + 1;
    }

    // Generate change notifications 
    SubscriberApi* item = m_subscribers.get();
    while ( item )
    {
        processSubscriptionEvent_( *item, eDATA_CHANGED );
        item = m_subscribers.get();
    }
}

/////////////////
void ModelPointCommon::attach( SubscriberApi& observer, uint16_t initialSeqNumber ) throw()
{
    m_modelDatabase.lock_();
    observer.setSequenceNumber_( initialSeqNumber );
    observer.setModelPoint_( this );
    processSubscriptionEvent_( observer, eATTACH );
    m_modelDatabase.unlock_();
}

void ModelPointCommon::detach( SubscriberApi& observer ) throw()
{
    m_modelDatabase.lock_();
    processSubscriptionEvent_( observer, eDETACH );
    observer.setModelPoint_( 0 );
    m_modelDatabase.unlock_();
}

/////////////////
void ModelPointCommon::processSubscriptionEvent_( SubscriberApi& subscriber, Event_T event ) throw()
{
    m_modelDatabase.lock_();

    switch ( (State_T) subscriber.getState_() )
    {
        case eSTATE_UNSUBSCRIBED:
            switch ( event )
            {
                case eATTACH:
                    transitionToSubscribed( subscriber );
                    break;

                case eDATA_CHANGED:
                    Cpl::System::FatalError::logf( "ModelPointCommon::processSubscriptionEvent_(): Data changed received when in the eSTATE_UNSUBSCRIBED state!" );
                    break;

                    // Ignore all other events
                default:
                    break;
            }
            break;

        case eSTATE_IDLE:
            switch ( event )
            {
                case eATTACH:
                    transitionToSubscribed( subscriber );
                    break;

                case eDETACH:
                    m_subscribers.remove( subscriber );
                    subscriber.setState_( eSTATE_UNSUBSCRIBED );
                    break;

                case eDATA_CHANGED:
                    // NOTE: By definition if the eDATA_CHANGED event was generated - the subscriber is NOT in the MP's subscribers list
                    transitionToNotifyPending( subscriber );
                    break;

                    // Ignore all other events
                default:
                    break;
            }
            break;

        case eSTATE_NOTIFY_PENDING:
            switch ( event )
            {
                case eATTACH:
                    subscriber.getMailbox_()->removePendingChangingNotification_( subscriber );
                    transitionToSubscribed( subscriber );
                    break;

                case eDETACH:
                    subscriber.getMailbox_()->removePendingChangingNotification_( subscriber );
                    subscriber.setState_( eSTATE_UNSUBSCRIBED );
                    break;

                case eNOTIFYING:
                    subscriber.setSequenceNumber_( m_seqNum );
                    subscriber.setState_( eSTATE_NOTIFY_NOTIFYING );
                    break;

                case eDATA_CHANGED:
                    Cpl::System::FatalError::logf( "ModelPointCommon::processSubscriptionEvent_(): Data changed received when in the eSTATE_NOTIFY_PENDING state!" );
                    break;

                    // Ignore all other events
                default:
                    break;
            }
            break;

        case eSTATE_NOTIFY_NOTIFYING:
            switch ( event )
            {
                case eDETACH:
                    subscriber.setState_( eSTATE_NOTIFY_PENDING_DETACH );
                    break;

                case eNOTIFY_COMPLETE:
                    transitionToSubscribed( subscriber );
                    break;

                case eDATA_CHANGED:
                    Cpl::System::FatalError::logf( "ModelPointCommon::processSubscriptionEvent_(): Data changed received when in the eSTATE_NOTIFY_NOTIFYING state!" );
                    break;

                    // Ignore all other events
                default:
                    break;
            }
            break;

        case eSTATE_NOTIFY_PENDING_DETACH:
            switch ( event )
            {
                case eATTACH:
                    subscriber.setState_( eSTATE_NOTIFY_NOTIFYING );
                    break;

                case eNOTIFY_COMPLETE:
                    subscriber.setState_( eSTATE_UNSUBSCRIBED );
                    break;

                 case eDATA_CHANGED:
                    Cpl::System::FatalError::logf( "ModelPointCommon::processSubscriptionEvent_(): Data changed received when in the eSTATE_NOTIFY_PENDING_DETACH state!" );
                    break;

                    // Ignore all other events
                default:
                    break;
            }
            break;
    }

    m_modelDatabase.unlock_();
}

void ModelPointCommon::transitionToSubscribed( SubscriberApi& subscriber ) throw()
{
    // Ensure that I am not already in the Model Point's list of subscribers (this can happen if subscribing when I am already subscribed)
    m_subscribers.remove( subscriber );

    if ( m_seqNum == subscriber.getSequenceNumber_() )
    {
        subscriber.setState_( eSTATE_IDLE );
        m_subscribers.put( subscriber );
    }
    else
    {
        return transitionToNotifyPending( subscriber );
    }
}

void ModelPointCommon::transitionToNotifyPending( SubscriberApi& subscriber ) throw()
{
    subscriber.getMailbox_()->addPendingChangingNotification_( subscriber );
    subscriber.setState_( eSTATE_NOTIFY_PENDING );
}


/////////////////
bool ModelPointCommon::testAndUpdateLock( LockRequest_T lockRequest ) throw()
{
    bool result = false;
    if ( lockRequest == eUNLOCK )
    {
        m_locked = false;
        result   = true;
    }
    else if ( lockRequest == eLOCK )
    {
        if ( m_locked == false )
        {
            m_locked = true;
            result   = true;
        }
    }
    else
    {
        if ( m_locked == false )
        {
            result = true;
        }
    }

    return result;
}


/////////////////
const char* ModelPointCommon::fromString( const char* srcText, const char* terminationChars, Cpl::Text::String* errorMsg, uint16_t* retSequenceNumber ) throw()
{
    // Trap prefix operation that apply to the Data 
    LockRequest_T lockAction    = eNO_REQUEST;
    int8_t        invalidAction = -1;
    srcText                     = parsePrefixOps( srcText, lockAction, invalidAction, terminationChars );

    // Invalidate request
    if ( invalidAction > 0 )
    {

        uint16_t seqnum = setInvalidState( invalidAction, lockAction );
        if ( retSequenceNumber )
        {
            *retSequenceNumber = seqnum;
        }
    }

    // Write action
    else
    {
        // No 'new value'
        if ( Cpl::Text::isCharInString( terminationChars, *srcText ) )
        {
            // Apply the lock request
            uint16_t seqnum = setLockState( lockAction );
            if ( retSequenceNumber )
            {
                *retSequenceNumber = seqnum;
            }
        }

        // New value
        else
        {
            // Set the element's value based on the source text
            srcText = setFromText( srcText, lockAction, terminationChars, errorMsg, retSequenceNumber );
        }
    }

    // Return the remaining 'un-parsed' text (or ZERO if any error)
    return srcText;
}

const char* ModelPointCommon::parsePrefixOps( const char* source, LockRequest_T& lockRequest, int8_t& invalidAction, const char* terminationChars )
{
    // Do nothing when there is NO 'source'
    if ( source == 0 || *source == '\0' )
    {
        return source;
    }

    // Remove leading whitespace
    source = Cpl::Text::stripSpace( source );

    // check for lock/unlock operations
    if ( *source == OPTION_CPL_RTE_MODEL_POINT_LOCK_CHAR )
    {
        lockRequest = eLOCK;
        source++;
    }
    else if ( *source == OPTION_CPL_RTE_MODEL_POINT_UNLOCK_CHAR )
    {
        lockRequest = eUNLOCK;
        source++;
    }
    else
    {
        lockRequest = eNO_REQUEST;
    }

    // Check for invalidate operation
    if ( *source == OPTION_CPL_RTE_MODEL_POINT_INVALID_CHAR )
    {
        const char* endPtr  = 0;
        int invalid         = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID;
        if ( !Cpl::Text::a2i( invalid, source + 1, 10, terminationChars, &endPtr ) || invalid < 1 || invalid > 127 )
        {
            // Force the default invalid state value on error (or when missing numeric value)
            invalid = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID;
        }

        invalidAction = (int8_t) invalid;
        source        = endPtr;
    }

    // Return the next 'un-parsed' character
    return source;
}

bool ModelPointCommon::convertStateToText( Cpl::Text::String& dstMemory, bool& append, bool isLocked, int8_t validState ) const throw()
{
    // Indicate locked state
    if ( isLocked )
    {
        dstMemory.formatOpt( append, "%c", OPTION_CPL_RTE_MODEL_POINT_LOCK_CHAR );
        append = true;
    }

    // All done if the element has a valid value
    if ( IS_VALID( validState ) )
    {
        return true;
    }

    // Invalid -->default invalid state
    if ( validState == OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID )
    {
        dstMemory.formatOpt( append, "%c", OPTION_CPL_RTE_MODEL_POINT_INVALID_CHAR );
    }

    // Invalid -->application specific value
    else
    {
        dstMemory.formatOpt( append, "%c%d", OPTION_CPL_RTE_MODEL_POINT_INVALID_CHAR, validState );
    }

    return false;
}
