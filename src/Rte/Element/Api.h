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
#include <stdlib.h>
#include <stdint.h>


/** This symbol provides the default 'Invalid' state value for an Element. The 
    application is free define/apply its own meaning to the set of 
    'invalid-values'. 
 */
#ifndef RTE_ELEMENT_API_STATE_INVALID
#define RTE_ELEMENT_API_STATE_INVALID   1
#endif

/// This symbol defines the 'Valid' state value for an Element
#define RTE_ELEMENT_API_STATE_VALID     0



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


public:
    /** This method sets the valid/invalid state of the element. A Value of zero
        indicates 'valid'; else ALL other values represent 'invalid'.  The 
        application is free define/apply its own meaning to the set of 
        'invalid-values' 
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
    /** This method copies the data content of the 'other' element to THIS
        instance.
     */
    virtual void copyDataFrom( const Api& other ) = 0;

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
    /// Virtual destructor to make the compiler happy
    virtual ~Api(){}
};

};      // end namespaces
};
#endif  // end header latch
