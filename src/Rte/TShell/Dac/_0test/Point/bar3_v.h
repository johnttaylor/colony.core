#ifndef Point_ViewerBar3_h_
#define Point_ViewerBar3_h_
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

#include "Rte/Point/Viewer/Container.h"
#include "Rte/TShell/Dac/_0test/Point/bar3.h"

///
namespace Point {


/** Concrete Viewer Point: BAR3
 */
template <class CONTEXT>
class ViewerBar3: public Bar3,
                  public Rte::Point::Viewer::Container<CONTEXT>
{
public:
    /// Constructor
    ViewerBar3( CONTEXT&                                                                    context,
                typename Rte::Point::Viewer::Container<CONTEXT>::ChangeNotificationFunc_T   contextChangedCb,
                typename Rte::Point::Viewer::Container<CONTEXT>::StoppedNotificationFunc_T  contextStoppedCb,
                ModelBar3&                                                                  modelPoint,
                Cpl::Itc::PostApi&                                                          viewerMbox 
              )
    :Rte::Point::Viewer::Container<CONTEXT>::Container(*this, context, contextChangedCb, contextStoppedCb, modelPoint, viewerMbox)
        {}
};

/** Concrete LIGHT WEIGHT Viewer Point: BAR3
 */
template <class CONTEXT>
class LWViewerBar3: public Bar3Null,
                    public Rte::Point::Viewer::Container<CONTEXT>
{
public:
    /// Constructor
    LWViewerBar3( CONTEXT&                                                                    context,
                  typename Rte::Point::Viewer::Container<CONTEXT>::ChangeNotificationFunc_T   contextChangedCb,
                  typename Rte::Point::Viewer::Container<CONTEXT>::StoppedNotificationFunc_T  contextStoppedCb,
                  ModelBar3&                                                                  modelPoint,
                  Cpl::Itc::PostApi&                                                          viewerMbox 
                )
    :Rte::Point::Viewer::Container<CONTEXT>::Container(*this, context, contextChangedCb, contextStoppedCb, modelPoint, viewerMbox)
        {}
};

};
#endif 