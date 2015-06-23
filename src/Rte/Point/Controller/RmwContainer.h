#ifndef Rte_Point_Controller_RmwContainer_h_
#define Rte_Point_Controller_RmwContainer_h_
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

#include "Rte/Point/Controller/Api.h"
#include "Rte/Point/Controller/RmwClient.h"
#include "Rte/Point/Model/Api.h"

///
namespace Rte { namespace Point { namespace Controller {


/** This template class is a composer pattern/class that manages the modify
    callback for a Read-Modify-Write Controller Container Point.
    
    The template arg 'TUPLE' is the Tuple type of the Point Container. The
    template arg 'CONTEXT' is a member function in the context class that
    is responsible for processing the traversal/modify callback methods.

    Note: The concrete 'TUPLE' class must inherit from Rte::Tupe::ContainerItem
          per the Point Container semantics/contract.
 */
template <class TUPLE, class CONTEXT>
class RmwContainer: public Api,
                    public RmwContainerClient,
                    public TUPLE
{
public:
    /** Define a callback method function for the Modify Point callback (See 
        Rte::Point::Controller::RmwContainerClient for additional details)
     */
    typedef int (CONTEXT::*ModifyFunc_T)(unsigned /* tupleIndex */, Rte::Tuple::Api*& /* modifiedTuplePtr */, bool& /* membershipChanged */);


protected:
    /// Reference to the Model point being modified
    Rte::Point::Model::Api&     m_modelPoint;
    
    /// Reference to my containing instance
    CONTEXT&                    m_context;

    /// Method (in my Context) to call to perform the modify operation
    ModifyFunc_T                m_modifyCb;

    /// Starting index for traversal
    unsigned                    m_startTupleIdx;


public:
    /// Constructor
    RmwContainer( CONTEXT&                  context,
                  ModifyFunc_T              modifyCallback,
                  Rte::Point::Model::Api&   modelPoint,
                  unsigned                  startingTupleIndex = 0
                );


public:
    /** This method allows the client to change the starting Tuple index for
        Traversal.  This method needs to be called BEFORE the call to 
        updateModel()
     */
    void setStartingTupleIndex( unsigned newStartIndex );

    /** This marks the 'current Tuple' is 'in-the-container'. This method 
        SHOULD ONLY be called inside the 'ModifyFunc_T' callback method. This
        method ALWAYS returns true - the intent is that return value is used
        to set 'membershipChanged' argument of the ModifyFunc_T callback
        method.
     */
    bool addItem();

    /** This marks the 'current Tuple' is NOT 'in-the-container'. This method 
        SHOULD ONLY be called inside the 'ModifyFunc_T' callback method. This
        method ALWAYS returns true - the intent is that return value is used
        to set 'membershipChanged' argument of the ModifyFunc_T callback
        method.
     */
    bool removeItem();


public:
    /// See Rte::Point::Controller::Api
    void updateModel( void );


public:
    /// See Rte::Point::Controller::RmwContainerClient
    int modifyItem( unsigned tupleIndex, Rte::Tuple::Api*& modifiedTuplePtr, bool& membershipChanged );

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <class TUPLE, class CONTEXT>
Rte::Point::Controller::RmwContainer<TUPLE, CONTEXT>::RmwContainer( CONTEXT&                  context,
                                                                    ModifyFunc_T              modifyCallback,
                                                                    Rte::Point::Model::Api&   modelPoint,
                                                                    unsigned                  startingTupleIndex 
                                                                  )
:m_modelPoint(modelPoint)
,m_context(context)
,m_modifyCb(modifyCallback)
,m_startTupleIdx(startingTupleIndex)
    {
    }


/////////////////
template <class TUPLE, class CONTEXT>
void Rte::Point::Controller::RmwContainer<TUPLE, CONTEXT>::setStartingTupleIndex( unsigned newStartIndex  )
    {
    m_startTupleIdx = newStartIndex;
    }
    

template <class TUPLE, class CONTEXT>
bool Rte::Point::Controller::RmwContainer<TUPLE, CONTEXT>::addItem( void )
    {
    m_inContainer_.set(true);
    m_inContainer_.setValid();
    m_inContainer_.setInUse();
    return true;
    }
    
template <class TUPLE, class CONTEXT>
bool Rte::Point::Controller::RmwContainer<TUPLE, CONTEXT>::removeItem( void )
    {
    m_inContainer_.set(false);
    m_inContainer_.setValid();
    m_inContainer_.setInUse();
    return true;
    }
    

/////////////////
template <class TUPLE, class CONTEXT>
void Rte::Point::Controller::RmwContainer<TUPLE, CONTEXT>::updateModel( void )
    {
    m_modelPoint.update( *this, *this, m_startTupleIdx );
    }
    

/////////////////
template <class TUPLE, class CONTEXT>
int Rte::Point::Controller::RmwContainer<TUPLE, CONTEXT>::modifyItem( unsigned tupleIndex, Rte::Tuple::Api*& modifiedTuplePtr, bool& membershipChanged )
    {
    // Modify the Tuple
    return (m_context.*m_modifyCb)( tupleIndex, modifiedTuplePtr, membershipChanged );
    }


};      // end namespaces
};
};
#endif  // end header latch
