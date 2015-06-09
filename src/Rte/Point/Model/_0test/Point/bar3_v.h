#ifndef Point_ViewerBar3_h_
#define Point_ViewerBar3_h_
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

#include "Rte/Point/Viewer/Container.h"
#include "Rte/Point/Model/_0test/Point/Bar3.h"

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
    ViewerBar3( CONTEXT&                                                                            context,
                typename Rte::Point::Viewer::Container<CONTEXT>::TupleChangeNotificationFunc_T      tupleChangedCb,
                typename Rte::Point::Viewer::Container<CONTEXT>::ContainerChangeNotificationFunc_T  containerChangedCb,
                typename Rte::Point::Viewer::Container<CONTEXT>::StoppedNotificationFunc_T          contextStoppedCb,
                Rte::Point::Model::Api&                                                             modelPoint,
                Cpl::Itc::PostApi&                                                                  viewerMbox 
              )
    :Rte::Point::Viewer::Container<CONTEXT>::Container(*this, context, tupleChangedCb, containerChangedCb, contextStoppedCb, modelPoint, viewerMbox)
        {}
};

};
#endif 