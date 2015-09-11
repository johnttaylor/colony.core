#ifndef Rte_Point_Controller_RmwComposer_h_
#define Rte_Point_Controller_RmwComposer_h_
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
    callback for a Read-Modify-Write Controller Point.
    
    A Composer is a structural pattern that may be used to employ composition 
    when implementing an interface rather than using multiple inheritance. This
    allows a single concrete object to receive change notifications via many
    Viewers.
 */
template <class CONTEXT>
class RmwComposer: public Api,
                   public RmwClient
{
public:
    /** Define a callback method function for the Modify Point callback (See 
        Rte::Point::Controller::RmwClient for additional details)
     */
    typedef void (CONTEXT::*ModifyFunc_T)(Rte::Point::Api*& /* modifiedPointPtr */);


protected:
    /// Reference to the client's Point that holds the result of the read 
    Rte::Point::Api&            m_myPoint;

    /// Reference to the Model point being modified
    Rte::Point::Model::Api&     m_modelPoint;
    
    /// Reference to my containing instance
    CONTEXT&                    m_context;

    /// Method (in my Context) to call to perform the modify operation
    ModifyFunc_T                m_modifyCb;


public:
    /// Constructor
    RmwComposer( Rte::Point::Api&          myPoint, 
                 CONTEXT&                  context,
                 ModifyFunc_T              modifyCallback,
                 Rte::Point::Model::Api&   modelPoint
               );


public:
    /// See Rte::Point::Controller::Api
    bool updateModel( void );


public:
    /// See Rte::Point::Controller::RmwClient
    void modify( Rte::Point::Api*& modifiedPointPtr );

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <class CONTEXT>
Rte::Point::Controller::RmwComposer<CONTEXT>::RmwComposer( Rte::Point::Api&          myPoint, 
                                                           CONTEXT&                  context,
                                                           ModifyFunc_T              modifyCallback,
                                                           Rte::Point::Model::Api&   modelPoint 
                                                         )
:m_myPoint(myPoint)
,m_modelPoint(modelPoint)
,m_context(context)
,m_modifyCb(modifyCallback)
    {
    }


/////////////////
template <class CONTEXT>
bool Rte::Point::Controller::RmwComposer<CONTEXT>::updateModel( void )
    {
    m_modelPoint.update( m_myPoint, *this );
    return true;
    }
    

/////////////////
template <class CONTEXT>
void Rte::Point::Controller::RmwComposer<CONTEXT>::modify( Rte::Point::Api*& modifiedPointPtr )
    {
    // Notify context
    (m_context.*m_modifyCb)( modifiedPointPtr );
    }


};      // end namespaces
};
};
#endif  // end header latch
