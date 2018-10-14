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
#include "Cpl/Rte/ModelDatabase.h"
#include "Cpl/Rte/SubscriberApi.h"
#include "Cpl/Container/DList.h"
#include <stdint.h>


///
namespace Cpl {
///
namespace Rte {


/** This concrete class provide common infrastructure for a Model Point.
 */
class ModelPointCommon_ : public ModelPoint
{
protected:
    /// List of Active Subscribers
    Cpl::Container::DList<SubscriberApi>    m_subscribers;

    /// Pointer to the Model Point's static information
    const StaticInfo&                       m_staticInfo;

    /// Reference to the containing Model Base
    ModelDatabase&                          m_modelDatabase;

    /// Reference to my Data
    void*                                   m_dataPtr;

    /// Sequence number used for tracking changes in the Point data
    uint16_t                                m_seqNum;

    /// Locked state
    bool                                    m_locked;

    /// Internal valid/invalid state
    int8_t                                  m_validState;


protected:
    /// Constructor
    ModelPointCommon_( ModelDatabase& myModelBase, void* myDataPtr, StaticInfo& staticInfo, int8_t validState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID );

public:
    /// See Cpl::Rte::ModelPoint
    const char* getName() const throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t getSequenceNumber() const throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t touch() throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest = eNO_REQUEST ) throw();

    /// See Cpl::Rte::ModelPoint
    int8_t getValidState( void ) const throw();

    /// See Cpl::Rte::ModelPoint
    bool isLocked() const throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t setLockState( LockRequest_T lockRequest ) throw();

    /// See Cpl::Rte::ModelPoint
    const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0, uint16_t* retSequenceNumber=0 ) throw();

    /// See Cpl::Rte::ModelPoint 
    size_t getExternalSize() const throw();

    /// See Cpl::Rte::ModelPoint.  Note: The implementation does NOT account for Endianess, i.e. assumes the 'platform' is the same for export/import
    size_t exportData( void* dstDataStream, size_t maxDstLength, uint16_t* retSequenceNumber = 0 ) const throw();

    /// See Cpl::Rte::ModelPoint.  Note: The implementation does NOT account for Endianess, i.e. assumes the 'platform' is the same for export/import
    size_t importData( const void* srcDataStream, size_t srcLength, uint16_t* retSequenceNumber = 0 ) throw();


protected:
    /// See Cpl::Rte::ModelPoint
    uint16_t read( void* dstData, size_t dstSize, int8_t& validState ) const throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t write( const void* srcData, size_t srcSize, LockRequest_T lockRequest = eNO_REQUEST ) throw();

    /// See Cpl::Rte::ModelPoint
    uint16_t readModifyWrite( GenericRmwCallback& callbackClient, LockRequest_T lockRequest = eNO_REQUEST );

    /// See Cpl::Rte::ModelPoint
    void attach( SubscriberApi& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw();

    /// See Cpl::Rte::ModelPoint 
    void detach( SubscriberApi& observer ) throw();


public:
    /// See Cpl::Container::DictItem
    const Cpl::Container::Key& getKey() const throw();

public:
    /// See Cpl::Rte::ModelPoint
    void processSubscriptionEvent_( SubscriberApi& subscriber, Event_T event ) throw();


protected:
    /** Internal helper method that completes the data update process as well
        as ensuring any change notifications get generated.  Note: This method
        ALWAYS sets the MP's state to 'valid'

        This method is NOT thread safe.
     */
    virtual void processDataUpdated() throw();

    /** Internal helper method that handles generating change notifications
        when the Model Point's data/state has changed.

        This method is NOT thread safe.
     */
    virtual void processChangeNotifications() throw();

    /** Internal helper method that advances/updates the Model Point's
        sequence number.

        This method is NOT thread safe.
     */
    virtual void advanceSequenceNumber() throw();

    /** Internal helper method that manages testing and updating the locked
        state.

        Rules:
        1) If 'lockRequest' is eNO_REQUEST, the method only returns true if
           the MP is in the unlocked state
        2) If 'lockRequest' is eLOCK, the method only returns if the MP is in
           the unlocked state.  In addition, when true is returned the MP is
           put into the locked state.
        3) If 'lockRequest' is eUNLOCK, the method always returns true and
           the MP is left in the unlocked state.

        This method is NOT thread safe.
     */
    virtual bool testAndUpdateLock( LockRequest_T lockRequest ) throw();


    /** Internal helper method that completes the data update process as well
        as ensuring any change notifications get generated AFTER a read-modify-
        write callback.

        This method is NOT thread safe.
     */
    virtual void processRmwCallbackResult( RmwCallbackResult_T result ) throw();


protected:
    /// Helper FSM method
    virtual void transitionToNotifyPending( SubscriberApi& subscriber ) throw();

    /// Helper FSM method
    virtual void transitionToSubscribed( SubscriberApi& subscriber ) throw();

protected:
    /** Helper method for encoding Invalid & Locked states.  Returns false
        when the MP's value is invalid; else true is returned.
     */
    virtual bool convertStateToText( Cpl::Text::String& dstMemory, bool& append, bool isLocked, int8_t validState ) const throw();

    /// Helper method that handles the lock/unlock/invalidate requests
    virtual const char* parsePrefixOps( const char* source, LockRequest_T& lockRequest, int8_t& invalidAction, const char* terminationChars );

    /** Helper method that set's the MP Data value from a text string.  Has the
        semantics of fromString() method.  This method MUST be implemented by
        the leaf child classes.
     */
    virtual const char* setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars=0, Cpl::Text::String* errorMsg=0, uint16_t* retSequenceNumber=0 ) throw() = 0;

};

};      // end namespaces
};
#endif  // end header latch
