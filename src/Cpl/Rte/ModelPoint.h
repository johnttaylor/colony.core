#ifndef Cpl_Rte_Model_Point_h_
#define Cpl_Rte_Model_Point_h_
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


#include "Cpl/Container/DictItem.h"
#include "Cpl/Text/String.h"
#include "Cpl/Rte/StaticInfo.h"
#include "Cpl/Rte/Point.h"
#include "Cpl/Rte/Subscriber.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Rte {


/** This mostly abstract class defines the interface for a Model Point.  A
    Model Point contains an instance of Cpl::Rte::Point and is responsible for
    managing thread safe access to the 'Point'.  The intent of a Model point is
    that it is the Application's canonical source for a 'Point'

    NOTES:
        1) The concrete leaf classes are responsible for ensuring type
           safety.
        2) All methods are atomic, i.e. a protected by the Model Base's mutex.
           This means that all methods will block if there is a call in-progress
           to ANY Model Point in a given Model Base.
        3) All methods in this class ARE thread Safe unless explicitly
           documented otherwise.
 */
class ModelPoint :
    public Cpl::Container::Key,
    public Cpl::Container::DictItem
{
public:
    /// Force levels
    enum Force_T
    {
        eNOT_FORCED   = 0,       //!< No 'force level' is applied (i.e. nominal write operations)
        eFORCE_LEVEL1 = 1,       //!< Lowest possible force level
        eFORCE_LEVEL2 = 2,       //!< Next highest force level
        eFORCE_LEVEL3 = 3,       //!< Next highest force level
        eFORCE_LEVEL4 = 4,       //!< Next highest force level
        eFORCE_LEVEL5 = 5,       //!< Next highest force level
        eFORCE_LEVEL6 = 6,       //!< Next highest force level
        eFORCE_LEVEL7 = 7,       //!< Next highest force level
        eFORCE_LEVEL8 = 8,       //!< Highest possible force level
    };


public:
    /** This class defines the callback interface for a Read-Modify-Write
        operation.
     */
    class RmwCallback
    {
    public:
        /// Possible results of the read-modify-write callback function
        enum Result_T
        {
            eNO_CHANGE,         //!< Indicates the callback function has NOT modify the Model Point's data.
            eCHANGED,           //!< Indicates the callback function has modify the Model Point's data
            eINVALIDATE,        //!< Request that the Model Point's data state be set to eINVALID
        };

    public:
        /** The callback function for the read-modify-write operation.  The
            function is responsible for being 'accurate' with respect to the
            result code.
         */
        virtual Result_T modelPointRmwCallback( Point& data, bool isValid ) throw() = 0;
    };

 

public:
    /** Magic value to use when registering for a change notification and
        application does not 'know' the current sequence number value of the 
        Model Point.
     */
    static const uint16_t SEQUENCE_NUMBER_UNKNOWN = 0;


public:
    /** This method returns the Model Point's name as a null terminated string.

        Note: Model Point names are assumed to be unique.  However, it is
              responsibility of the Application to enforce this constraint.  If
              names are not unique then the look-up by Model Point name
              functionality will fail.  No other functionality depends on have
              unique names.
     */
    virtual const char* getName() const throw() = 0;

    /** This method returns the Model Point's current sequence number.
     */
    virtual uint16_t getSequenceNumber() const throw() = 0;

    /** This method returns true when the MP data is invalid.
     */
    virtual bool isNotValid() const throw() = 0;

    /** This method does NOT alter the MP's data or set, but unconditionally
        triggers the MP change notification(s). The method returns the Model
        Point's sequence number after the method completes.
     */
    virtual uint16_t touch() throw() = 0;

    /** This method sets the Model Point's state to invalid. Note: Any write
        operation will set the Model Point's state to valid.

        Calling this method when the Model Point's state is already invalid,
        the method has no effect, e.g. the Model Point's sequence number is not
        changed.

        Change Notification(s) will be triggered any time there is a transition
        in the MP state (valid <--> invalid).
     */
    virtual uint16_t setInvalid() throw() = 0;

    /** This method unconditionally removes all force levels from the MP, but
        does not change the MP data/state. If the MP is currently not in a
        forced state - nothing is done.  This method never triggers change
        notification(s).

        NOTE: The Application is responsible for enforcing its own force level
              usage constraints, e.g. who should/allowed-to be calling the
              remove force level methods?
    */
    virtual void removeAllForceLevels() throw() = 0;

    /** This method removes a single force level from the MP, but does not
        change the MP data/state. If the MP is currently not in a forced state
        or the specified force level is not active - nothing is done.  This
        method never triggers change notification(s).

        NOTE: Essential a 'list' of all active force level is maintained.
              This means than when removing the highest active force level,
              the MP's force level reverts to the next active highest force
              level set.
     */
    virtual void removeForceLevel( Force_T forceLevelToRemove ) throw() = 0;


protected:
    /** This method copies the Model Point's content to the caller's Point
        instance. The method returns the Model Point's sequence number after
        the method completes.

        If 'isValid' is returned as false then contents of 'dst' is meaningless.

        Notes:
        1) The assumption is that Model Point's internal data and 'dst' are of
           the same Point leaf class type.
        2) The data size of the 'src' Point instance is ALWAYS honored when
           coping the data from the Model Point
        3) The Model Point's sequence number is not changed.
     */
    virtual uint16_t read( Point& dst, bool& isValid ) const throw() = 0;

    /** This method writes the caller Point instance to the Model Point's
        internal data.  The method returns the Model Point's sequence number
        after the method completes.

        Model Point supports the concept of a client 'forcing' the MP's data
        value.  When a MP's data has been forced - any attempted writes to the
        MP will SILENTLY fail.  Once the MP's data is locked - only write calls
        with the original forceLevel or higher can update the MP's data.  The
        Application uses the removeForce() command to undo/remove a active
        force level.

        Notes:
        1) The assumption is that Model Point's internal data and 'src' are of
           the same Point leaf class type.
        2) The data size of the Model Points data instance is ALWAYS honored
           when coping the data from 'src'
     */
    virtual uint16_t write( const Point& src, Force_T forceLevel = eNOT_FORCED ) throw() = 0;

    /** This method is used to perform a Read-Modify-Write operation on the
        Model Point's data.  The method returns the Model Point's sequence
        number after the method completes.

        The callback will not be called (and no update to the MP data occurs)
        if the specified 'forceLevel' is not sufficient to allow a write
        operation.  When the callback method is called, the specified force
        level will only be applied to the MP if the callback methods returns
        eCHANGED or eINVALIDATE.

        This method allows a caller to read, write, read/write all or potions
        of the Model Point's data. The caller's callback function should be
        kept as short as possible as the entire Model Base is locked during the
        callback.

        Typically this method is for an atomic read/write operation or when
        creating a Point instance on the stack for reading/writing is
        prohibitive.

        NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
              potential to lockout access to the ENTIRE Model Base for an
              indeterminate amount of time.  And alternative is to have the
              concrete Model Point leaf classes provide the application
              specific read, write, read-modify-write methods in addition or in
              lieu of the read/write method in this interface.
     */
    virtual uint16_t readModifyWrite( RmwCallback& callbackClient, Force_T forceLevel = eNOT_FORCED ) = 0;

    /** This method removes a single force level from the MP AND potentially
        updates the MP's data in a single atomic operation. See the truth table
        for additional details.  The method returns the MP's sequence number
        (after the updated occurred) if the MP data was updated; else if the MP
        data was NOT updated then SEQUENCE_NUMBER_UNKNOWN is returned.

        Note: the 'forceLevelToRemove' is ALWAYS removed, regardless of whether
              or not the MP was updated
        <pre>

        MP active force level   forceLevelToRemove  | MP data updated
        ---------------------   ------------------  | ----------------
              not-forced            1-8             |    true
              1-8                   >= activeLevel  |    true
              1-8                   < activeLevel   |    false

        </pre>

        This method will only trigger change notification(s) when it returns
        a valid sequence number (i.e. not SEQUENCE_NUMBER_UNKNOWN) AND there was
        actual change in the MP data/state.
     */
    virtual uint16_t removeForceLevel( Force_T forceLevelToRemove, const Point& src ) throw() = 0;

    /** This method is used to attach a subscriber to a Model Point.  Once 
        attached the Subscriber will receive a change notification (aka a 
        callback) every time the Model Point's data/state changes. Once, a
        Subscriber is attached - it will stay attached to the application
        calls detach().

        There is no limit to the number of Subscribers that can attach to
        a Model Point.

        The attach() method can be called even if the Subscriber is already 
        attached.  When this happens, the attach process is 'restarted', i.e.
        the 'initialSeqNumber' is used for the Subscriber's sequence number.

        The change-detect mechanism uses a sequence number.  Each Model Point
        and each Subscriber has sequence number.  When the Subscriber's sequence
        number does not equals the Model Point's sequence number - the Subscriber
        receives a change notification and the Subscriber's sequence number is
        updated to match the Model Point's sequence number at the time of
        change notification.  When a Subscriber attaches to Model Point with
        an the 'seqNumber' argument set to SEQUENCE_NUMBER_UNKNOWN, the
        Subscriber will get an 'immediate' change notification.

        The callbacks for the Change Notifications are called as part of the 
        RTE's Mailbox server.  As part of the asynchronous processing (timers,
        ITC, EventFlags, etc.) of the RTE Mailbox server will also process
        all pending Change Notification and invoke the call backs.  What does
        that all mean?  The Change notifications are "local" the Subscribers
        thread very similar to how the Cpl::Timers work.  It also means that
        no change notification callback will be called till the Mailbox server
        loops back to the "top" of its forever loop.
     */
    virtual void attach( Subscriber& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw() = 0;

    /** This method is used to detach a Subscriber to a Model Point.  See the
        attach() method for more details about the Subscription/Change 
        Notification mechanism.

        The detach() method can be called even if the Subscriber is NOT
        currently attached.  The detach() method can be called within the
        Change Notification callback.
     */
    virtual void detach( Subscriber& observer ) throw() = 0;


public:
    /// Subscriber events
    enum Event_T
    {
        eATTACH,            //!< The Application is requesting to subscribe to a model point
        eDETACH,            //!< The Application is requesting to un-subscribe from the model point
        eDATA_CHANGED,      //!< The model point's data/state has change a pending change notification is needed
        eNOTIFYING,         //!< The subscriber's change notification callback is being called
        eNOTIFY_COMPLETE    //!< The subscriber's change notification callback has been completed
    };

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.
        
        This method is used by Model Point to process events related to the 
        subscription/change-notification process

        This method is Thread Safe
     */
    virtual void processSubscriptionEvent_( Subscriber& subscriber, Event_T event ) throw() =0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~ModelPoint() {}
};

};      // end namespaces
};
#endif  // end header latch
