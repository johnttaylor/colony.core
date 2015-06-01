#ifndef Rte_Tuple_Api_h_
#define Rte_Tuple_Api_h_
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


#include "Rte/Element/Api.h"
#include <stdlib.h>
#include <stdint.h>


///
namespace Rte { namespace Tuple {


/** This class defines the basic operations that can be performed
    on a Model Tuple.
 */
class Api
{
public:
    /** This method returns the number of Elements contain in the tuple
     */
    virtual unsigned getNumElements(void) const = 0;

    /** This method returns a refernce to the specified element.  The element
        index is zero based index.

        NOTE: If the specified element does not exist, a fatal error
              will be generated
     */
    virtual Rte::Element::Api& getElement( unsigned elementIdx ) const = 0;

    /// Same as getElement()
    inline Rte::Element::Api& operator[] (unsigned i) const { return getElement(i); }


public:
    /** This class method performs a copy operation for a tuple.  If 
        'inUseFilerP' is zero, then all elements in the tuple are copied;
        else then only elements that are marked by inUseFilter->isInUse()
        are copied.
     */
    static void copy( Api& dst, const Api& src, const Api* inUseFilterPtr=0 );

    /** This class method performs a compare of two tuples and if there
        are different, then 'src' id copied to 'dst'.  The default behavior for
        the compare operation is to compare the internal tuples' sequence
        numbers for differences.  Tuples in the Model have their sequence number
        incremented on every 'write' operation.  The 'compareValues' flag will 
        force a comparison of Tuples' actual values. The default operation is
        to only compare/copy the "in-use" elements of the 'dst' tuple. To
        compare/copy all elements set 'allElements' to true.   If there was 
        difference in the two tuples true is returned.
     */
    static bool compareAndCopy( Api& src, Api& dst, bool allElements, bool compareValues=true );


public:
    /** Helper method that sets the valid state for ALL Elements to the
        same state.
     */
    virtual void setAllValidFlagState( bool newState ) = 0;

    /** Helper method that sets the in use state for ALL Elements to the
        same state.
     */
    virtual void setAllInUseState( bool newState ) = 0;

    /** Helper method that returns the SUM of ALL the element's export
        data size.
     */
    virtual size_t sumAllExternalSize(void) const = 0;
    

public:
    /** This method returns true when one or more elements in the tuple
        have been updated.  Typically this method is used by a viewer.
     */
    virtual bool isUpdated(void) const = 0;

    /** This method clears the updated state.  Typically this method is ONLY
        used by the model to mark a newly registered 'viewer' as not-changed
     */
    virtual void clearUpdatedState(void) = 0;

    /** This method sets the updated state.  Typically this method is ONLY
        used by the model when it generates a change notification
     */
    virtual void setUpdatedState(void) = 0;


public:
    /** Increments the Tuple's internal sequence number.  The sequence number
        is used to track any/all 'writes' made to the Tuple
     */
    virtual void incrementSequenceNumber(void) = 0;

    /// Returns the Tuple's internal sequence number
    virtual uint32_t getSequenceNumber(void) const = 0;

    /// Sets the Tuple's sequence number
    virtual void setSequenceNumber( uint32_t newSeqNum ) = 0;

    /// Invaldates the Tuple's internal sequence number
    virtual void invalidateSequenceNumber(void) = 0;

    /// Returns true if there is a different by two Tuples - as determined by the Sequence number
    virtual bool isDifferent( Api& other ) const = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api(){}
};





};      // end namespaces
};
#endif  // end header latch
