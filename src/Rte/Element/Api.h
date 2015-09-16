#ifndef Rte_Element_Api_h_
#define Rte_Element_Api_h_
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


#include "colony_config.h"
#include "Rte/Element/DataType.h"
#include "Cpl/Text/String.h"
#include <stdlib.h>
#include <stdint.h>


/** This symbol provides the default 'Invalid' state value for an Element. The 
    application is free define/apply its own meaning to the set of 
    'invalid-values'.  NOTE: All 'Status' values MUST be a POSITIVE integer, 
    i.e. between 0 and 127.  Negative values ARE Reserved by the RTE Engine.  
 */
#ifndef RTE_ELEMENT_API_STATE_INVALID
#define RTE_ELEMENT_API_STATE_INVALID   1
#endif

/// This symbol defines the 'Valid' state value for an Element
#define RTE_ELEMENT_API_STATE_VALID     0


/** Quote character used for ALL "text" elements when encoding/decoding to text.
    A "text" element is an Element that whose value when converted to text can 
    contain special ASCII characters such as commas, spaces, quotes, question 
    mark, dollar sign, semi-colon, etc.
 */
#ifndef OPTION_RTE_ELEMENT_QUOTE_CHAR
#define OPTION_RTE_ELEMENT_QUOTE_CHAR        '"'
#endif

/// Escape character used for ALL String element when encoding/decoding to text.
#ifndef OPTION_RTE_ELEMENT_ESCAPE_CHAR
#define OPTION_RTE_ELEMENT_ESCAPE_CHAR       '`'
#endif




///
namespace Rte { namespace Element {


/** This class defines the basic operations that can be performed
    on ALL types of Model Elements.
 */
class Api
{
public:
    /** This method returns the data type that THIS instance's data content.
     */
    virtual const DataType getType(void) const = 0;

    /** This method returns a string identifier for the Element's data type.
        Note: This value is NOT guaranteed to be unique -->use the DataType
        class for uniquess -->this is just a helper method for display type
        values as text.
     */
     virtual const char* getTypeAsText(void) const = 0;


public:
    /** This method sets the valid/invalid state of the element. A Value of zero
        indicates 'valid'; else ALL other values represent 'invalid'.  The 
        application is free define/apply its own meaning to the set of 
        'invalid-values'.  The value MUST be a POSITIVE integer, i.e. between
        0 and 127.  Negative values ARE Reserved by the RTE Engine.
     */
    virtual void setValidState( int8_t newState ) = 0;

    /// This method is used to mark the element's data as invalid
    inline void setInvalid(void) { setValidState(RTE_ELEMENT_API_STATE_INVALID); }

    /// This method is used to mark the element's data as valid
    inline void setValid(void)   { setValidState(RTE_ELEMENT_API_STATE_VALID); }

    /// Returns true if element has a valid/known value
    virtual bool isValid(void) const = 0;

    /// Returns the Element's actual Valid/Invalid state value
    virtual int8_t validState(void) const = 0;



public:
    /** Returns true if the element is marked as 'in-use'. The in-use
        flag is used by Viewers, Controllers, and Queries to exclude
        the Element (when 'in-use' state is false) from the read/write
        operation being performed.
     */
    virtual bool isInUse(void) const = 0;

    /// This method sets the 'in use' state
    virtual void setInUseState( bool newState ) = 0;

    /// This method is used to mark the element as 'in use'
    inline void setInUse(void)      { setInUseState(true); }

    /// This method is used to mark the element as 'not in use'
    inline void setNotInUse(void)   { setInUseState(false); }


public:
    /** Returns true if the element is marked as 'locked'.  In the locked
        state - ALL WRITE/UPDATE OPERATIONS (except for changing the locked
        state) are silently ignored/dropped.
     */
    virtual bool isLocked(void) const = 0;

    /** This method sets the Element to the locked state.  The Application 
        should NEVER call this method directly -->it should use the 'requestXxxx'
        method below instead.
     */
    inline void setLocked(void)     { setLockedState( true ); }

    /** This method sets the Element to the unlocked state.  The Application 
        should NEVER call this method directly -->it should use the 'requestXxxx'
        method below instead.
     */
    inline void setUnlocked(void)   { setLockedState( false ); }  

    /** This method is use to the 'locked' state.  The application should NEVER
        call this method directly -->it should use the requestLockOperation
        method below.
     */
    virtual void setLockedState( bool newState ) = 0;


public:
    /** Returns true if element has been marked with a 'lock request'
     */
    virtual bool isLockRequest(void) const = 0;

    /** Returns true if element has been marked with a 'lock unrequest'
     */
    virtual bool isUnlockRequest(void) const = 0;

    /** This method sets the 'locked request' state of the element.  When
        'newState' is true a lock operation is requested; else a unlock
        operation is requested.  Only Controllers can/should/do make lock/unlock 
        requests. 
        
        Notes: 
        
            o The 'inUse' flag has NO effect on the request lock operations, 
              i.e. the lock/unlock request operations are orthogonal to 
              the 'inUse' flag.
            o The application CAN update and lock an element is a single
              Controller update action.  However, the application can NOT
              unlock and update an elemetn is single Controller update
              action.  A second/subsequent Controller update action is
              required to update a Element once the Element has been
              unlocked.
     */
    virtual void requestLockOperation( bool newState ) = 0;

    ///This method is used to request the element be 'locked'. 
    inline void requestLocked(void)       { requestLockOperation(true); }

    /// This method is used to request the element be 'unlocked'
    inline void requestUnlocked(void)     { requestLockOperation(false); }

    /** This method clears any previous Lock/Unlock operation requests.
        Note: Typically the Application and/or Controller does NOT need
              to call this method.  This method is ALWAYS internally called
              once the lock request operaiton has bee applied to the
              model Point/Tuple/Element.
     */
    virtual void clearLockOperation(void) = 0;


public:
    /** This method copies the data content of the 'other' element to THIS
        instance. This method returns true when the copy operation was
        performed, i.e. the element is NOT in the locked state; else false
        is returned.
     */
    virtual bool copyDataFrom( const Api& other ) = 0;

    /** This method returns true IF the data content of the THIS instance
        is NOT equal to the 'other' elements data content .
     */
    virtual bool isDifferentFrom( const Api& other ) const = 0;

    /** This method returns a pointer to the element's data content.
        NOTE: Be VERY CAUTIOUS in using the method.  The returned pointer value
              has very limited scope, i.e. not guarentied to remain valid
              if other operations are performed on the Element.
     */
    virtual const void* dataPointer( void ) const = 0;


public:
    /** This method is used to export THIS instance's data content
        and valid state to a raw data stream.  It is the responsiblity of 
        the caller to ensure that there is sufficient memory availabe for 
        the data being exported.  The method returns the number of bytes 
        exported.
     */
    virtual size_t exportElement( void* dstDataStream ) const = 0;

    /** This method is used to populate the data content and valid state
        from the a raw data stream/pointer.  It is the responsibility of the 
        caller to ensure that the data stream is appropriate for element type 
        and that the data stream content was originally created by the
        corresponding exportElement() method. The method returns the number of 
        bytes consumed from the data stream.
     */
    virtual size_t importElement( const void* srcDataStream ) = 0;

    /** Returns the size, in bytes, of the element's data content and
        valid flag.
        NOTE: The size returned is the size of the element data WHEN it
              is being exported/imported - this is NOT the value of
              the size of the element's internal storage use for the
              data content and valid flag.
     */
    virtual size_t externalSize(void) const = 0;


public:
    /** This method returns the Element's string representation of its
        value.  The caller must supply the destination string to hold
        the returned value.
     */
    virtual const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const = 0;

    /** This method attempts to set the Element's value from the provided
        text string.  When 'terminationChars' is not null/0, the converstion
        of text to binary is stopped if one of characters in 'terminationChars'
        is encountered.  When 'terminationChars' is null/0, the entire string
        contents (i.e. till '\0' is found) is converted. If the conversion 
        successful a point to next character after the last 'consumed' 
        charactered is returned.  If the contents of the 'srcText' is invalid 
        OR the Element does not support a full/complete conversion from Text to 
        binary then the method returns 0.
     */
    virtual const char* setFromText( const char* srcText, const char* terminationChars=0 ) = 0;


public:
    /** This is an INTERNAL method to the RTE Engine for managing the
        various states/status of an Element -->the Application should NEVER
        call this method.
     */
    virtual void setRawValidState_( int8_t newRawState ) = 0;

    /** This is an INTERNAL method to the RTE Engine for managing the
        various states/status of an Element -->the Application should NEVER
        call this method.
     */
    virtual int8_t getRawValidState_(void) const = 0;


public:
    /** This method returns true of the Element is a 'Model Element' vs.
        an Element in a Controller, Viewer, etc.
     */
    virtual bool isModelElement(void) const = 0;

    /** This is an INTERNAL method to the RTE Enginer for marking an
        Element as a Model-Element.  The default construction is the 
        Element is not-a-Model-Element.  Once set it cannot be changed.  
        The Application should NEVER calls this method.
     */
    virtual void setAsModelElement_(void) = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api(){}
};

};      // end namespaces
};
#endif  // end header latch
