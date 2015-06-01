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
#ifndef _eden_devtests_mvc_server_point_foo2_viewer_h_
#define _eden_devtests_mvc_server_point_foo2_viewer_h_

#include "eden/devtests/mvc/server/points/foo2.h"
#include "eden/mvc/point/viewer/composer.h"

///
namespace DevTest {
///
namespace Points {


/** Concrete Viewer Point: BAR2
 */
template <class Context>
class ViewerBar2: public Bar2,
                  public Eden::MVC::Point::Viewer::Composer<Context>
{
public:
    /// Constructor
    ViewerBar2( Context&                        context,
                typename Eden::MVC::Point::Viewer::Composer<Context>::ChangeNotificationFunc_t        contextChangedCb,
                typename Eden::MVC::Point::Viewer::Composer<Context>::StoppedNotificationFunc_t       contextStoppedCb,
                Eden::MVC::Point::Model::Api&   modelPoint,
                Jcl::PostMessageApi&            viewerMbox )
              :Eden::MVC::Point::Viewer::Composer<Context>::Composer(*this,context,contextChangedCb,contextStoppedCb,modelPoint,viewerMbox)
              {}
};

};
};
#endif // end _eden_devtests_mvc_server_point_foo2_viewer_h_