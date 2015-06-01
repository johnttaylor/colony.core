#ifndef Rte_Point_Api_h_
#define Rte_Point_Api_h_
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


#include "Cpl/Container/Item.h"
#include "Rte/Tuple/Api.h"


///
namespace Rte { namespace Point {


/** This class defines the basic operations that can be performed
    on a Model Point.

    NOTE: For 'other' Points in the method below MUST have the same
          tuple/elements signature as this Point's tuple/elements or 
          a FATAL error will be generated.
 */
class Api: public Cpl::Container::Item
{
public:
    
    /** This method is used to update the Point's internal tuple(s) and or
        element(s).  The data content of each element will be update as 
        well as the element's valid/invalid state. If 'inUseFilterP' is zero,
        then all elements are updated; else the in-use flags specified by
        the point pointed to by 'inUseFilterP' is use to only copy elements
        that are marked as in-use.
     */
    virtual void copyFrom( Api& other, Api* inUseFilterP=0 ) = 0;

    /** This method compares the tuple in the 'other' point and if there is
        difference, then the 'other' point's tuple(s) are updated from this
        point. The 'compareValues' determines if the comparision of the Point's
        data is done using a change-sequence-number or by comparing actual 
        Point values. The default operation is to only  compare/copy the
        "in-use" elements of the 'other' points. To compare/copy all elements
        set 'allElements' to true.   If there was difference in the two points, 
        then true is returned.
     */
    virtual bool compareAndCopy( Api& other, bool allElements=false, bool compareValues=true ) = 0;


public:
    /** Helper method that sets the valid state for ALL Elements in ALL 
        Tuples to the same state.
     */
    virtual void setAllValidFlagState( bool newState ) = 0;

    /** Helper method that sets the in use state for ALL Elements in ALL
        Tuples to the same state.
     */
    virtual void setAllInUseState( bool newState ) = 0;

    /** Helper method that returns the SUM of ALL the element's export
        data size in ALL of the Tuples.
     */
    virtual size_t sumAllExternalSize(void) = 0;


public:
    /** This method returns the number of tuples contain in the Point
     */
    virtual unsigned getNumTuples(void) const = 0;

    /** This method returns a refernce to the specified tuple.  The tuple
        index is zero based index.

        NOTE: If the specified tuple does not exist, a fatal error
              will be generated
     */
    virtual Rte::Tuple::Api& getTuple( unsigned tupleIdx ) = 0;

    /// Same as getTuple()
    inline Rte::Tuple::Api& operator[] (unsigned i) { return getTuple(i); }


public:
    /** This method returns true when one or more Tuples in the Point
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
    /** Increments the Point's internal sequence number.  The sequence number
        is used to track any/all 'writes/changes' made to the Point
     */
    virtual void incrementSequenceNumber(void) = 0;

    /// Returns the Point's internal sequence number
    virtual uint32_t getSequenceNumber(void) const = 0;

    /// Sets the Point's sequence number
    virtual void setSequenceNumber( uint32_t newSeqNum ) = 0;

    /// Invaldates the Point's internal sequence number
    virtual void invalidateSequenceNumber(void) = 0;

    /// Returns true if there is a different by two Points - as determined by the Sequence number
    virtual bool isDifferent( Api& other ) const = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~Api(){}
};

};      // end namespaces
};
#endif  // end header latch
