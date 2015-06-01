/*----------------------------------------------------------------------------- 
* This file is part of the Shift-Right Open Repository.  The Repository is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the root directory or on the Internet at           
* http://www.shift-right.com/openrepo/license.htm                               
*                                                                               
* Copyright (c) 2001-2010 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
#ifndef _eden_devtests_mvc_server_point_foo1_viewercontroller_h_
#define _eden_devtests_mvc_server_point_foo1_viewercontroller_h_

#include "eden/devtests/mvc/server/points/foo1.h"
#include "eden/mvc/point/viewercontroller/base.h"

///
namespace DevTest {
///
namespace Points {


/** Concrete Viewer/Controller Point: BAR1
 */
template <class Context>
class ViewerControllerBar1: public Eden::MVC::Point::ViewerController::Base<Context>
{
public:
    ///
    Bar1    _viewer;
    ///
    Bar1    _controller;

public:
    /// Constructor
    ViewerControllerBar1( Context&                         context,
                          typename Eden::MVC::Point::Viewer::Composer<Context>::ChangeNotificationFunc_t         contextChangedCb,
                          typename Eden::MVC::Point::Viewer::Composer<Context>::StoppedNotificationFunc_t        contextStoppedCb,
                          Eden::MVC::Point::Model::Api&    modelPoint,
                          Jcl::PostMessageApi&             viewerMbox 
                        )
                        :Eden::MVC::Point::ViewerController::Base<Context>::Base(_viewer,context,contextChangedCb,contextStoppedCb,modelPoint,viewerMbox,_controller)
                            {
                            }
};

};
};
#endif // end _eden_devtests_mvc_server_point_foo1_viewercontroller_h_