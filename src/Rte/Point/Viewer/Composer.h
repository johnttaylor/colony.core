#ifndef Rte_Point_Viewer_Composer_h_
#define Rte_Point_Viewer_Composer_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */


#include "Rte/Point/Viewer/Base.h"


///
namespace Rte { namespace Point { namespace Viewer {


/** This template class is a composer pattern/class that manages the change
    notifications for a Point Viewer.
    
    A Composer is a structural pattern that may be used to employ composition 
    when implementing an interface rather than using multiple inheritance. This
    allows a single concrete object to receive change notifications via many
    Viewers.
 */
template <class CONTEXT>
class Composer: public Base
{
public:
    /// Define a callback method function for when Model Point changes
    typedef void (CONTEXT::*ChangeNotificationFunc_T)(void);

    /** Define a callback method function for when the viewer has been
        be successfully stopped.
     */
    typedef void (CONTEXT::*StoppedNotificationFunc_T)(void);


protected:
    /// Reference to my containing instance
    CONTEXT&                        m_context;

    /// Method (in my Context) to call when a change notificaiton is received
    ChangeNotificationFunc_T        m_changedCb;

    /// Method (in my Context) to call when the viewer has been stopped
    StoppedNotificationFunc_T       m_stoppedCb;


protected:
    /// Constructor
    Composer( Rte::Point::Api&          myPoint, 
              CONTEXT&                  context,
              ChangeNotificationFunc_T  contextChangedCb,
              StoppedNotificationFunc_T contextStoppedCb,
              Rte::Point::Model::Api&   modelPoint,
              Cpl::Itc::PostApi&        viewerMbox 
            );


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
Rte::Point::Viewer::Composer<CONTEXT>::Composer( Rte::Point::Api&          myPoint, 
                                                 CONTEXT&                  context,
                                                 ChangeNotificationFunc_T  contextChangedCb,
                                                 StoppedNotificationFunc_T contextStoppedCb,
                                                 Rte::Point::Model::Api&   modelPoint,
                                                 Cpl::Itc::PostApi&        viewerMbox 
                                               )
:Base(myPoint, modelPoint, viewerMbox)
,m_context(context)
,m_changedCb(contextChangedCb)
,m_stoppedCb(contextStoppedCb)
    {
    }

/////////////////
template <class CONTEXT>
void Rte::Point::Viewer::Composer<CONTEXT>::modelHasChanged( void )
    {
    // Notify context
    (m_context.*m_changedCb)();
    }
    
template <class CONTEXT>
void Rte::Point::Viewer::Composer<CONTEXT>::viewerHasStopped( void )
    {
    // Notify context
    (m_context.*m_stoppedCb)();
    }


};      // end namespaces
};
};
#endif  // end header latch
