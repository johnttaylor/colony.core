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
#include "Cpl/Rte/SubscriberApi.h"
#include <stdint.h>

/** This symbol provides the default 'Invalid' state value for a Model Point. 
    The application is free define/apply its own meaning to the set of 
    'invalid-values'.  NOTE: All 'State' values MUST be a POSITIVE integer, 
    i.e. between 0 and 127.  Negative values ARE Reserved by the RTE Engine.  
 */
#ifndef OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID
#define OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID   1
#endif


///
namespace Cpl {
///
namespace Rte {


/** This mostly abstract class defines the interface for a Model Point.  A
    Model Point contains an instance of a Point's data and is responsible for
    managing thread safe access to the 'Point'.  The intent of a Model point is
    that it is the Application's canonical source for a 'Point'

    NOTES:
        1) The concrete leaf classes are responsible for ensuring type
           safety.
        2) All methods are atomic, i.e. a protected by the Model Base's mutex.
           This means that all methods will block if there is a call in-progress
           to ANY Model Point in a given Model Data Base.
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
    /// Possible results of the read-modify-write callback function
    enum RmwCallbackResult_T
    {
        eNO_CHANGE,         //!< Indicates the callback function has NOT modify the Model Point's data.
        eCHANGED,           //!< Indicates the callback function has modify the Model Point's data
        eINVALIDATE,        //!< Request that the Model Point's data state be set to eINVALID
    };

public:
    /// Defines the generic, non-type safe read-modify-write client callback interface
    class GenericRmwCallback
    {
    public:
        // Generic callback for the readModifyWrite() operation
        virtual RmwCallbackResult_T genericCallback( void* data, int8_t validState ) throw() = 0;

    public:
        /// Virtual destructor
        ~GenericRmwCallback() {}
    };


public:
    /** Magic value to use when registering for a change notification and
        application does not 'know' the current sequence number value of the
        Model Point.
     */
    static const uint16_t SEQUENCE_NUMBER_UNKNOWN = 0;

    /// This symbol defines the 'Valid' state value for a Model Point
    static const int8_t MODEL_POINT_STATE_VALID   = 0;


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

    /** This method does NOT alter the MP's data or set, but unconditionally
        triggers the MP change notification(s). The method returns the Model
        Point's sequence number after the method completes.
     */
    virtual uint16_t touch() throw() = 0;

    /** This method returns the RAM size, in bytes, of the Model Point's data.
      */
    virtual size_t getSize() const throw() = 0;


public:
    /** This method sets the invalid state of the Model Point. Any value  
        greater zero indicates represent 'invalid'. If a zero or negative
        values is specified, the method will treat the value as 
        'OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID'. 
        
        The application is free define/apply its own meaning to the set of 
        'invalid-values'.  The value MUST be a POSITIVE integer, i.e. between
        0 and 127.  Zero and negative values ARE Reserved by the RTE Engine.

        The method returns the Model Point's sequence number after updating
        the valid state.

        Notes:
        1) Any write operation will set the Model Point's state to valid.
        2) Change Notification(s) will ONLY be triggered when there is valid to
           invalid state transition (i.e. if the Model Point is already in 
           the invalid state - no change notification will be triggered).

     */
    virtual uint16_t setInvalidState( int8_t newInvalidState ) throw() = 0;

    /// Returns the Model Point's actual Valid/Invalid state value
    virtual int8_t getValidState(void) const throw() = 0;

    /// This method returns true when the MP data is invalid.
    inline bool isNotValid() const throw()  { return getValidState() != MODEL_POINT_STATE_VALID; }

    /// This method is used to mark the element's data as invalid
    inline void setInvalid(void) throw()    { setInvalidState(OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID); }

    /** Short-hand method to improve readability for testing the a return invalid
        state for 'valid'
     */
    inline static bool IS_VALID( int8_t validState ) { return validState == MODEL_POINT_STATE_VALID; }

    
public:
    /** This method converts the Model Point's data to a a string value and 
        copies the resultant string into 'dst'.  If the Model Point's data 
        cannot be represented as a string then the contents of 'dst' is set to 
        an empty string and the method returns false; else the method returns 
        true. The format of the string is specific to the concrete leaf class.  
        However, it is strongly recommended that the output of this method be 
        the same format that is expected for the fromString() method.
       
        NOTE: If the converted string is larger than the memory allocated by
              'dst' then the string result in 'dst' will be truncated. The
              caller is required to check the truncated() method of 'dst' to
              check for the truncated scenario.
     */
    virtual bool toString( Cpl::Text::String& dst, bool append=false, uint16_t* retSequenceNumber=0 ) const throw() = 0;

    /** This method attempts to convert the null terminated 'src' string to
        its binary format and copies the result to the Model Point's internal 
        data. The expected format of the string is specific to the concrete 
        leaf class.

        When 'terminationChars' is not 0, the conversion from text to binary is
        stopped if one of characters in 'terminationChars' is encountered.
        When 'terminationChars' is 0, the entire string contents (i.e. till
        '\0' is found) is converted.

        If the conversion is successful a pointer to next character after the
        last 'consumed' charactered is returned.  If the contents of the 'src'
        is invalid, OR the Point does not support a full/complete conversion
        from Text to binary, OR the conversion fails then the method returns 0.  
        When the conversion fails, the optional 'errorMsg' argument is updated 
        with a plain text error message.
     */
    virtual const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0, uint16_t retSequenceNumber=0 ) throw() = 0;

    /** This method returns the maximum size, in bytes not including the null
        terminator, of the string returned by the toString() method.
     */
    virtual size_t getToStringMaxSize() throw() = 0;



public:
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


public:
    /** This method is used to export the Model Point's instance data content
        to a raw data stream.  It is the responsibility of the caller to ensure
        that there is sufficient memory available for the data being exported.
        The method returns the number of bytes exported.

        The method optionally return the Model Point's sequence number at the
        time of the export.
     */
    virtual size_t export(void* dstDataStream, uint16_t* retSequenceNumber=0 ) const throw() = 0;

    /** This method is used to populate the Model Point's data content from the 
        a raw data stream/pointer.  It is the responsibility of the caller to 
        ensure that the data stream is appropriate for element type and that 
        the data stream content was originally created by the corresponding 
        export() method. The method returns the number of bytes consumed from 
        the data stream.

        The method optionally return the Model Point's sequence number once the
        import has completed.

        The method ALWAYS triggers a change notification(s) for the Model Point
     */
    virtual size_t import( const void* srcDataStream, uint16_t* retSequenceNumber=0 ) throw() = 0;

    /** Returns the size, in bytes, of the element's data content.

        NOTE: The size returned is the size of the Point data WHEN it
              is being exported/imported - this is NOT the value of
              the size of the Point's internal storage use for the
              data content.
     */
    virtual size_t getExternalSize() const = 0;
    


protected:
    /** This method copies the Model Point's content to the caller's Point
        instance. The method returns the Model Point's sequence number after
        the method completes.

        If 'validState' indicates that the data is invalid, then contents of 
        'dst' is meaningless.

        Notes:
        1) The assumption is that Model Point's internal data and 'dstData' are 
           of the same type.
        2) The data size of the 'dstSize' is ALWAYS honored when coping the 
           data from the Model Point
        3) The Model Point's sequence number is not changed.
     */
    virtual uint16_t read( void* dstData, size_t dstSize, int8_t& validState ) const throw() = 0;

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
        1) The assumption is that Model Point's internal data and 'srcData' are 
           of the same type.
        2) The data size of the Model Points data instance is ALWAYS honored
           when coping the data from 'srcData'
     */
    virtual uint16_t write( const void* srcData, Force_T forceLevel = eNOT_FORCED ) throw() = 0;

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
              lieu of the read/write methods in this interface.
     */
    virtual uint16_t readModifyWrite( GenericRmwCallback& callbackClient, Force_T forceLevel = eNOT_FORCED ) = 0;

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

        The method returns the Model Point's sequence number after the method
        completes.  The sequence number will have only changed if the MP was
        updated.
     */
    virtual uint16_t removeForceLevel( Force_T forceLevelToRemove, const void* srcData ) throw() = 0;

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
        that all mean?  The Change notifications are "local" to the Subscriber's
        thread very similar to how the Cpl::Timers work.  It also means that
        no change notification callback will be called till the Mailbox server
        loops back to the "top" of its forever loop.
     */
    virtual void attach( SubscriberApi& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw() = 0;

    /** This method is used to detach a Subscriber to a Model Point.  See the
        attach() method for more details about the Subscription/Change
        Notification mechanism.

        The detach() method can be called even if the Subscriber is NOT
        currently attached.  The detach() method can be called within the
        Change Notification callback.
     */
    virtual void detach( SubscriberApi& observer ) throw() = 0;


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
    virtual void processSubscriptionEvent_( SubscriberApi& subscriber, Event_T event ) throw() =0;


public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is used to unconditionally update the Model Point's data.

        Notes:
        1) The assumption is that Model Point Data instance and 'src' are the 
           of the same type.
        2) The internal data size of the Model Point instance is ALWAYS honored 
           when coping the data from 'src'
        3) The Model Point's sequence number is not changed.
    */

    virtual void copyDataFrom_( const void* srcData ) throw() = 0;

 
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is used update's the caller's 'Point Data' with the Model 
        Point's data.

        Notes:
        1) The assumption is that Model Point Data instance and 'dst' are the 
           of the same type.
        2) The 'dstSize' of the destination ALWAYS honored when coping the 
           Model Point's data to 'dst'
        3) The Model Point's sequence number is not changed.
    */

    virtual void copyDataTo_( void* dstData, size_t dstSize ) const throw() = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method compares the Model Point's data to the data of 'other' Model
        Point and returns true if the data of both points are the same. It is 
        assumed that Model Point instance and 'other' are the of the same leaf 
        class type.
     */
    virtual bool isDataEqual_( const void* otherData ) const throw() = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method returns a pointer to the Model Point's data.  BE VERY 
        CAREFULL on how the pointer is used!
     */
    virtual void* getDataPointer_() const throw() = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~ModelPoint() {}
};

};      // end namespaces
};
#endif  // end header latch
