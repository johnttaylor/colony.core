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
#ifndef _eden_devtests_mvc_server_point_foo3_viewer_h_
#define _eden_devtests_mvc_server_point_foo3_viewer_h_

#include "eden/devtests/mvc/server/points/foo3.h"
#include "eden/mvc/point/viewer/container.h"

///
namespace DevTest {
///
namespace Points {


/** Concrete Viewer Point: BAR3
 */
template <class Context>
class ViewerBar3: public Bar3,
                  public Eden::MVC::Point::Viewer::Container<Context>
{
public:
    /// Constructor
    ViewerBar3( Context&                          context,
                typename Eden::MVC::Point::Viewer::Container<Context>::TupleChangeNotificationFunc_t     tupleChangedCb,
                typename Eden::MVC::Point::Viewer::Container<Context>::ContainerChangeNotificationFunc_t containerChangedCb,
                typename Eden::MVC::Point::Viewer::Container<Context>::StoppedNotificationFunc_t         contextStoppedCb,
                Eden::MVC::Point::Model::Api&     modelPoint,
                Jcl::PostMessageApi&              viewerMbox )
              :Eden::MVC::Point::Viewer::Container<Context>::Container(*this,context,tupleChangedCb,containerChangedCb,contextStoppedCb,modelPoint,viewerMbox)
              {}
};

};
};
#endif // end _eden_devtests_mvc_server_point_foo3_viewer_h_