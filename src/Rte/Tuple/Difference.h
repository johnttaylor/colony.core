#ifndef Rte_Tuple_Difference_h_
#define Rte_Tuple_Difference_h_
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


#include <stdlib.h>


///
namespace Rte { namespace Tuple {


/** This class defines the interface for light-weight (vs comparing all
    Elements values) comparision mechnaism.  A sequence number is assocaited
    with each Tuple and the sequence number is updated everytime the Tuple
    is updated.  When the sequence number of a Viewer (or Querier) Tuple is
    different from a Model Tuple - a change/difference is declared.  

    NOTE: Because the Model Tuple's sequence number is updated EVERY time 
          Tuple is 'written to' - this Difference mechansim does NOT 
          distinguish between writing a new value vs re-writing the 
          exist value. 
 */
class Difference
{
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
    virtual ~Difference(){}
};





};      // end namespaces
};
#endif  // end header latch
