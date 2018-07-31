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

/// Subscriber States
enum State_T
{
    eSTATE_UNSUBSCRIBED = 0,          // Not subscribed to any model point
    eSTATE_IDLE,                      // Subscribed and waiting for a change
    eSTATE_NOTIFY_PENDING,            // Subscribed and waiting for next change notification dispatch cycle
    eSTATE_NOTIFY_NOTIFYING,          // The Client change notification callback is being executed
    eSTATE_NOTIFY_PENDING_DETACH,     // The subscription was requested to be cancelled during the change notification callback 
};


////////////////////////
ModelPointCommon::ModelPointCommon( ModelBase& myModelBase, Point& myData, StaticInfo* staticInfo, bool isValid )
    : m_staticInfo( staticInfo )
    , m_modelBase( myModelBase )
    , m_data( myData )
    , m_seqNum( SEQUENCE_NUMBER_UNKNOWN )
    , m_valid( isValid )
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
    m_modelBase.lock();
    uint16_t result = m_seqNum;
    m_modelBase.unlock();
    return result;
}

bool ModelPointCommon::isValid() const throw()
{
    m_modelBase.lock();
    bool result = m_valid;
    m_modelBase.unlock();
    return result;
}

uint16_t ModelPointCommon::read( Point& dst, bool& isValid ) const throw()
{
    m_modelBase.lock();
    isValid = m_valid;
    if ( isValid )
    {
        dst.copyFrom( m_data );
    }
    uint16_t result = m_seqNum;
    m_modelBase.unlock();

    return result;
}

uint16_t ModelPointCommon::write( const Point& src, Force_T forceLevel ) throw()
{
    m_modelBase.lock();
    if ( testAndSetForceLevel( forceLevel ) )
    {
        if ( !m_valid || m_data.isEqual( src ) == false )
        {
            m_data.copyFrom( src );
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelBase.unlock();

    return result;
}

uint16_t ModelPointCommon::readModifyWrite( RmwCallback& callbackClient, Force_T forceLevel )
{
    m_modelBase.lock();
    if ( testAndSetForceLevel( forceLevel ) )
    {
        // Invoke the client's callback function
        RmwCallback::Result_T result = callbackClient.modelPointRmwCallback( m_data, m_valid );

        // Do nothing if the callback did not change anything
        if ( result != RmwCallback::eNO_CHANGE )
        {
            // Handle request to invalidate the MP data
            if ( result == RmwCallback::eINVALIDATE )
            {
                if ( m_valid )
                {
                    m_valid = false;
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
    m_modelBase.unlock();

    return result;
}

uint16_t ModelPointCommon::touch() throw()
{
    m_modelBase.lock();
    processChangeNotifications();
    uint16_t result = m_seqNum;
    m_modelBase.unlock();
    return result;
}

uint16_t ModelPointCommon::setInvalid() throw()
{
    m_modelBase.lock();
    if ( m_valid )
    {
        m_valid = false;
        processChangeNotifications();
    }

    uint16_t result = m_seqNum;
    m_modelBase.unlock();
    return result;
}

/////////////////
void ModelPointCommon::removeAllForceLevels() throw()
{
    m_modelBase.lock();
    m_forceLevel = 0;
    m_modelBase.unlock();
}

void ModelPointCommon::removeForceLevel( Force_T forceLevelToRemove ) throw()
{
    if ( forceLevelToRemove != eNOT_FORCED )
    {
        uint8_t bitMask = 1 << (forceLevelToRemove - 1);
        m_modelBase.lock();
        m_forceLevel &= ~bitMask;
        m_modelBase.unlock();
    }
}

uint16_t ModelPointCommon::removeForceLevel( Force_T forceLevelToRemove, const Point& src ) throw()
{
    m_modelBase.lock();
    if ( testAndClearForceLevel( forceLevelToRemove ) )
    {
        if ( !m_valid || m_data.isEqual( src ) == false )
        {
            m_data.copyFrom( src );
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelBase.unlock();

    return result;
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
    m_valid = true;
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
    Subscriber* item = m_subscribers.get();
    while ( item )
    {
        processSubscriptionEvent_( *item, eDATA_CHANGED );
        item = m_subscribers.get();
    }
}

/////////////////
void ModelPointCommon::attach( Subscriber& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw()
{
    observer.setSequenceNumber_( initialSeqNumber );
    observer.setModelPoint_( this );
    processSubscriptionEvent_( observer, eATTACH );
}

void ModelPointCommon::detach( Subscriber& observer ) throw()
{
    processSubscriptionEvent_( observer, eDETACH );
    observer.setModelPoint_( 0 );
}

/////////////////
void ModelPointCommon::processSubscriptionEvent_( Subscriber& subscriber, Event_T event ) throw()
{
    m_modelBase.lock();

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

    m_modelBase.unlock();
}

void ModelPointCommon::transitionToSubscribed( Subscriber& subscriber ) throw()
{
    // Ensure that I am not already in the Model Point's list of subscribers (this can happen if subscribe when I am already subscribed)
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

void ModelPointCommon::transitionToNotifyPending( Subscriber& subscriber ) throw()
{
    subscriber.getMailbox_().addPendingChangingNotification_( subscriber );
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
