#ifndef Rte_Point_Viewer_Container_h_
#define Rte_Point_Viewer_Container_h_
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


#include "Rte/Point/Viewer/Base.h"


///
namespace Rte { namespace Point { namespace Viewer {

/** This template class is a composer pattern/class that manages the change
    notifications for a Point Container Viewer. Note: The Point being viewed 
    MUST be of type: Rte::Point::ContainerApi
    
    A Composer is a structural pattern that may be used to employ composition 
    when implementing an interface rather than using multiple inheritance. This
    allows a single concrete object to receive change notifications via many
    Viewers.
 */
template <class CONTEXT>
class Container: public Base
{
public:
    /** Defines a callback method function for when Membership in 
        the Model Point changes.  A change in Membership is defined
        as a change of state on the one or mores tuple's m_inContainer_
        Elements.  When there is membership change in the container,
        this method is guaranteed to be called AFTER the 
        TupleChangeNotificationFunc_T callback. 

        NOTE: All Container membership changes ARE also Tuple changes
              i.e. both the ContainerChangeNotificationFunc_T and
              TupleChangeNotificationFunc_T callback methods will ALWAYS
              be called when there is membership change.  HOWEVER, a
              change to a Tuple that is already 'in the container' will
              ONLY trigger the TupleChangeNotificationFunc_T callback.
     */
    typedef void (CONTEXT::*ContainerChangeNotificationFunc_T)(void);

    /** Defines a callback method function for when a Tuple within 
        the Model Point changes.
     */
    typedef void (CONTEXT::*TupleChangeNotificationFunc_T)(void);

    /** Define sa callback method function for when the viewer has been
        be successfully stopped.
     */
    typedef void (CONTEXT::*StoppedNotificationFunc_T)(void);


protected:
    /// Reference to my containing instance
    CONTEXT&                            m_context;

    /// Method (in my Context) to call when a Tuple change notificaiton is received
    TupleChangeNotificationFunc_T       m_changedCb;

    /// Method (in my Context) to call when there is change in membmership in the Model's Container Point
    ContainerChangeNotificationFunc_T   m_containerChangedCb;
    
    /// Method (in my Context) to call when the viewer has been stopped
    StoppedNotificationFunc_T           m_stoppedCb;


protected:
    /** Constructor.
        Notes: 
            o If tupleChangedCb is zero, then no callback will occur
            o if containerChangedCb is zero, then no callback will occur
     */
    Container( Rte::Point::Api&                  myContainerPoint, 
               CONTEXT&                          context,
               TupleChangeNotificationFunc_T     tupleChangedCb,
               ContainerChangeNotificationFunc_T containerChangedCb,
               StoppedNotificationFunc_T         contextStoppedCb,
               Rte::Point::Model::Api&           modelPoint,
               Cpl::Itc::PostApi&                viewerMbox 
             );

public:
    /// See Rte::Point::Viewer::Api
    unsigned startViewing( bool forceInitialUpdate = true, bool useValueForDifference = true );


protected:
    /// See Rte::Point::Viewer::Base
    void modelHasChanged( void );

    /// See Rte::Point::Viewer::Base
    void viewerHasStopped( void );

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <class CONTEXT>
Rte::Point::Viewer::Container<CONTEXT>::Container( Rte::Point::Api&                  myContainerPoint, 
                                                   CONTEXT&                          context,
                                                   TupleChangeNotificationFunc_T     tupleChangedCb,
                                                   ContainerChangeNotificationFunc_T containerChangedCb,
                                                   StoppedNotificationFunc_T         contextStoppedCb,
                                                   Rte::Point::Model::Api&           modelPoint,
                                                   Cpl::Itc::PostApi&                viewerMbox 
                                                 )
:Base( myContainerPoint, modelPoint, viewerMbox ),
 m_context( context ),
 m_changedCb( tupleChangedCb ),
 m_containerChangedCb( containerChangedCb ),
 m_stoppedCb( contextStoppedCb )
    {
    }

/////////////////
template <class CONTEXT>
unsigned Rte::Point::Viewer::Container<CONTEXT>::startViewing( bool forceInitialUpdate, bool useValueForDifference )
    {
    getMyPoint().resetSequenceNumber();
    return Base::startViewing(forceInitialUpdate, useValueForDifference);
    }
    
template <class CONTEXT>
void Rte::Point::Viewer::Container<CONTEXT>::modelHasChanged( void )
    {
    // Notify context
    if ( m_changedCb )
        {
        (m_context.*m_changedCb)();
        }
    if ( m_containerChangedCb && getMyPoint().isMembershipChanged() )
        {
        (m_context.*m_containerChangedCb)();
        }
    }
    
template <class CONTEXT>
void Rte::Point::Viewer::Container<CONTEXT>::viewerHasStopped( void )
    {
    // Notify context
    (m_context.*m_stoppedCb)();
    }

};      // end namespaces
};
};
#endif  // end header latch
