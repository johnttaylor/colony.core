#ifndef Cpl_Rte_ModelPointBase_h_
#define Cpl_Rte_ModelPointBase_h_
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


#include "Cpl/Rte/ModelPoint.h"
#include "Cpl/Rte/ModelBase.h"
#include "Cpl/Rte/SubscriberApi.h"
#include "Cpl/Container/DList.h"


///
namespace Cpl {
///
namespace Rte {


/** This concrete class provide common infrastructure for a Model Point.
 */
class ModelPointCommon : public ModelPoint
{
protected:
    /// List of Active Subscribers
    Cpl::Container::DList<SubscriberApi>    m_subscribers;

    /// Pointer to the Model Point's static information
    const StaticInfo*                       m_staticInfo;

    /// Reference to the containing Model Base
    ModelBase&                              m_modelBase;

    /// Reference to my Data
    Point&                                  m_data;

    /// Sequence number used for tracking changes in the Point data
    uint16_t                                m_seqNum;

    /// Force level
    uint8_t                                 m_forceLevel;

    /// Internal valid/invalid state
    bool                                    m_valid;


protected:
    /// Constructor
    ModelPointCommon( ModelBase& myModelBase, Point& myData, StaticInfo* staticInfo, bool isValid = false );

public:
    /// See Cpl::Rte::ModelPoint
    const char* getName() const throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t getSequenceNumber() const throw();

    /// See Cpl::Rte::ModelPoint
    bool isValid() const throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t touch() throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t setInvalid() throw();

    /// See Cpl::Rte::ModelPoint
    void removeAllForceLevels() throw();

    /// See Cpl::Rte::ModelPoint
    void removeForceLevel( Force_T forceLevelToRemove ) throw();


protected:
    /// See Cpl::Rte::ModelPoint
    uint16_t read( Point& dst, bool& isValid ) const throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t write( const Point& src, Force_T forceLevel = eNOT_FORCED ) throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t readModifyWrite( GenericRmwCallback& callbackClient, Force_T forceLevel = eNOT_FORCED );

    /// See Cpl::Rte::ModelPoint
    uint16_t removeForceLevel( Force_T forceLevelToRemove, const Point& src ) throw();

    /// See Cpl::Rte::ModelPoint
    void attach( SubscriberApi& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw();

    /// See Cpl::Rte::ModelPoint 
    void detach( SubscriberApi& observer ) throw();

public:
    /// See Cpl::Container::Key
    int compareKey( const Key& key ) const;

    /// See Cpl::Container::Key
    const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const;


public:
    /// See Cpl::Rte::ModelPoint
    void processSubscriptionEvent_( SubscriberApi& subscriber, Event_T event ) throw();


protected:
    /** Internal helper method that completes the data update process as well
        as ensuring any change notifications get generated.  Note: This method
        ALWAYS sets the MP's state to 'valid'

        This method is NOT thread safe.
     */
    void processDataUpdated() throw();

    /** Internal helper method that handles generating change notifications
        when the Model Point's data/state has changed.

        This method is NOT thread safe.
     */
    void processChangeNotifications() throw();

    /** Internal helper method that checks if the specified force level has
        sufficient privileges to write to the Model Point's data and returns
        true if it does. In addition, when the method returns trues AND the
        specified force level is eFORCE_LEVEL0 or higher privilege the internal
        force level is updated to the new specified force level.

        This method is NOT thread safe.
     */
    bool testAndSetForceLevel( Force_T forceLevel ) throw();

    /** Internal helper method that checks if the specified force level has
        sufficient privileges to write to the Model Point's data and returns
        true if it does. In addition the method ALWAYS clears the specified
        'forceLevel'

        This method is NOT thread safe.
     */
    bool testAndClearForceLevel( Force_T forceLevel ) throw();

    /** Helper method that returns the highest actively set force level.

        This method is NOT thread safe.
     */
    Force_T ModelPointCommon::getHighestForceLevel() const throw();


protected:

    /// Helper FSM method
    void transitionToNotifyPending( SubscriberApi& subscriber ) throw();

    /// Helper FSM method
    void transitionToSubscribed( SubscriberApi& subscriber ) throw();

};

};      // end namespaces
};
#endif  // end header latch
