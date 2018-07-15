#ifndef Rte_Point_Controller_RmwClient_h_
#define Rte_Point_Controller_RmwClient_h_
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

#include "Rte/Point/Api.h"
#include "Cpl/Type/Traverser.h"


///
namespace Rte { namespace Point { namespace Controller {

/** This abstract class defines the callback interface for modifying
    entire non-container Point during the updateModel() process.
 */
class RmwClient 
{
public:
    /** This method is called after the model point has been 'queried'.
        If the client modifies the model point -  it must set 
        'modifiedPoint' pointer to the point instance containing the new data 
        - so that on return from this call the model point  will be 
        updated. If the 'modifiedPoint' pointer is set to zero, then
        no modification of the model point will occur (the model sets
        'modifiedPoint' argument to zero prior to each call).

        NOTES:

            o When the read/queried operaiton is performed ALL Elements
              are read/copied, i.e. no 'in-use' settings are used
        
            o The 'in-use' settings/flags of 'modifiedPoint' are used
              to determine which Elements are written to

            o The client needs to MINIMIZE the amount of time spend during
              the modify since all other operations on the Model are blocked
              until the modify completes (to be precise - the ITC Controller 
              msg must run to completion before the thread its Model Point is
              executing in will process the next ITC message).
     */
    virtual void modify( Rte::Point::Api*& modifiedPointPtr ) = 0;

public: 
    /// Virtual destructor to make the compiler happy
    virtual ~RmwClient(void){}
};



/** This abstract class defines the callback interface for modifying
    individual items/tuples in Container Point during the updateModel()
    process.
 */
class RmwContainerClient 
{
public:
    /// Magic values that effect the traversal sequence
    enum { eNEXT=-1, eDONE=-2 };


public:
    /** This method is called once for every item/tuple in the Model 
        Container Point being updated. The contents of the model point's 
        tuple is first copied to the client's tuple BEFORE this method is 
        called.  If client modifies the item/tuple - it must set 
        'modifiedTuple' pointer to the tuple instance containing the new data 
        - so that on return from this call the model point's tuple will be 
        updated. If the 'modifiedTuple' pointer is set to zero, then
        no modification of the model's point will occur (the model sets
        'modifiedTuple' argument to zero prior to each call).

        If the client changes a Tuple membership status during this callback
        method - the client MUST set the 'membershipChanged' flag to true before
        returning from the callback.
        
        The traversal starts with index 0.  However the client controls 
        the traversal sequence by returning the index of 'next' item/tuple it 
        wants the call back for.  There are two special returns values -
        eNEXT and eDONE - that will advance to next index or abort the 
        traversal respectively.  The traversal stops when an index that is 
        greater than the number of container items or eDONE is returned.

        NOTES:

            o When the read/queried operaiton is performed ALL Elements
              are read/copied, i.e. no 'in-use' settings are used
        
            o The 'in-use' settings/flags of 'modifiedPoint' are used
              to determine which Elements are written to
        
            o The client needs to MINIMIZE the amount of time spend during
              the traversal since all other operations on the Model are blocked
              until the traversal completes (to be precise - the ITC Controller 
              msg must run to completion before the thread its Model Point is
              executing in will process the next ITC message).
     */
    virtual int modifyItem( unsigned tupleIndex, Rte::Tuple::Api*& modifiedTuplePtr, bool& membershipChanged ) = 0;


public: 
    /// Virtual destructor to make the compiler happy
    virtual ~RmwContainerClient(void){}
};

};      // end namespaces
};
};
#endif  // end header latch
