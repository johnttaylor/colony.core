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
ModelPointCommon::ModelPointCommon( ModelDatabase& myModelBase, void* myDataPtr, StaticInfo* staticInfo, int8_t validState )
    : m_staticInfo( staticInfo )
    , m_modelDatabase( myModelBase )
    , m_dataPtr( myDataPtr )
    , m_seqNum( SEQUENCE_NUMBER_UNKNOWN )
    , m_validState( validState )
    , m_forceLevel( 0 )
{
}

/////////////////
const char* ModelPointCommon::getName() const throw()
{
    return m_staticInfo->getName();
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

uint16_t ModelPointCommon::setInvalidState( int8_t newInvalidState ) throw()
{
    // Force a 'valid Invalid State value
    if ( newInvalidState <= 0 )
    {
        newInvalidState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID;
    }

    m_modelDatabase.lock_();
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
        copyDataTo_( m_dataPtr, dstSize );
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t ModelPointCommon::write( const void* srcData, Force_T forceLevel ) throw()
{
    m_modelDatabase.lock_();
    if ( testAndSetForceLevel( forceLevel ) )
    {
        if ( !IS_VALID( m_validState ) || isDataEqual_( srcData ) == false )
        {
            copyDataFrom_( srcData );
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t ModelPointCommon::readModifyWrite( GenericRmwCallback& callbackClient, Force_T forceLevel )
{
    m_modelDatabase.lock_();
    if ( testAndSetForceLevel( forceLevel ) )
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
void ModelPointCommon::removeAllForceLevels() throw()
{
    m_modelDatabase.lock_();
    m_forceLevel = 0;
    m_modelDatabase.unlock_();
}

void ModelPointCommon::removeForceLevel( Force_T forceLevelToRemove ) throw()
{
    if ( forceLevelToRemove != eNOT_FORCED )
    {
        uint8_t bitMask = 1 << (forceLevelToRemove - 1);
        m_modelDatabase.lock_();
        m_forceLevel &= ~bitMask;
        m_modelDatabase.unlock_();
    }
}

uint16_t ModelPointCommon::removeForceLevel( Force_T forceLevelToRemove, const void* srcData ) throw()
{
    m_modelDatabase.lock_();
    if ( testAndClearForceLevel( forceLevelToRemove ) )
    {
        if ( !IS_VALID( m_validState ) || isDataEqual_( srcData ) == false )
        {
            copyDataFrom_( srcData );
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

/////////////////
size_t ModelPointCommon::export(void* dstDataStream, uint16_t* retSeqNum ) const throw()
{
    size_t result = 0;
    if ( dstDataStream )
    {
        m_modelDatabase.lock_();
        
        // Export Data
        size_t dataSize = getSize();
        memcpy( dstDataStream, getDataPointer_(), dataSize );
        
        // Export Valid State
        uint8_t* ptr = (uint8_t*) dstDataStream;
        memcpy( ptr+dataSize, &m_validState, sizeof( m_validState ) );

        // Return the Sequence number when requested
        if ( retSeqNum )
        {
            *retSeqNum = m_seqNum;
        }

        m_modelDatabase.unlock_();
        result = getExternalSize();
    }
    return result;
}

size_t ModelPointCommon::import( const void* srcDataStream, uint16_t* retSeqNum ) throw()
{
    size_t result = 0;
    if ( srcDataStream )
    {
        m_modelDatabase.lock_();

        // Import Data
        size_t dataSize = getSize();
        memcpy( getDataPointer_(), srcDataStream, dataSize );

        // Import Valid State
        uint8_t* ptr = (uint8_t*) srcDataStream;
        memcpy( &m_validState, ptr+dataSize, sizeof( m_validState ) );

        // Generate change notifications and return the Sequence number when requested
        processDataUpdated();
        if ( retSeqNum )
        {
            *retSeqNum = m_seqNum;
        }

        m_modelDatabase.unlock_();
        result = getExternalSize();
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
    return m_staticInfo->compareKey( key );
}

const void* ModelPointCommon::getRawKey( unsigned* returnRawKeyLenPtr ) const
{
    return m_staticInfo->getRawKey( returnRawKeyLenPtr );
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
void ModelPointCommon::attach( SubscriberApi& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw()
{
    observer.setSequenceNumber_( initialSeqNumber );
    observer.setModelPoint_( this );
    processSubscriptionEvent_( observer, eATTACH );
}

void ModelPointCommon::detach( SubscriberApi& observer ) throw()
{
    processSubscriptionEvent_( observer, eDETACH );
    observer.setModelPoint_( 0 );
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
bool ModelPointCommon::testAndSetForceLevel( Force_T forceLevel ) throw()
{
    bool result = false;
    if ( m_forceLevel == 0 )
    {
        result = true;
        if ( forceLevel != eNOT_FORCED )
        {
            m_forceLevel = 1 << (forceLevel - 1);
        }
    }
    else
    {
        // Check if the new force level is sufficient
        if ( forceLevel >= getHighestForceLevel() )
        {
            result        = true;
            m_forceLevel |= 1 << (forceLevel - 1);
        }
    }

    return result;
}

bool ModelPointCommon::testAndClearForceLevel( Force_T forceLevel ) throw()
{
    bool result = false;
    if ( m_forceLevel == 0 )
    {
        result = true;
    }
    else
    {
        // Check if the new force level is sufficient
        if ( forceLevel >= getHighestForceLevel() )
        {
            result = true;
        }
    }

    // Clear the force level
    uint8_t bitMask = 1 << (forceLevel - 1);
    m_forceLevel &= ~bitMask;

    return result;
}

ModelPoint::Force_T ModelPointCommon::getHighestForceLevel() const throw()
{
    // Find the highest force level that is current set
    unsigned mask;
    int      idx;
    for ( mask=0x80, idx=8; idx > 0; idx--, mask >>= 1 )
    {
        if ( (m_forceLevel & mask) == mask )
        {
            return (Force_T) idx;
        }
    }

    return eNOT_FORCED;
}
