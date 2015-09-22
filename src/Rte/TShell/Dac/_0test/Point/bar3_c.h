#ifndef Point_ControllerBar3_h_
#define Point_ControllerBar3_h_
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

#include "Rte/Point/Controller/Container.h"
#include "Rte/Point/Controller/RmwContainer.h"
#include "Rte/Point/Controller/TupleItem.h"
#include "Rte/TShell/Dac/_0test/Point/bar3_m.h"

///
namespace Point {


/** Concrete Controller Point: BAR3
 */
class ControllerBar3: public Bar3,
                      public Rte::Point::Controller::Container
{
public:
    /// Constructor
    ControllerBar3( ModelBar3& modelPoint )
        :Rte::Point::Controller::Container(*this, modelPoint)
            {
            }

};


/** Concrete Tuple ITEM Controller Point: BAR3::Foo3
 */
class TupleItemControllerBar3: public Tuple::Foo3, 
                               public Rte::Point::Controller::TupleItem<Tuple::Foo3>
{
public:
    /// Constructor
    TupleItemControllerBar3( unsigned myTupleItemIdx, ModelBar3& modelPoint )
        :Rte::Point::Controller::TupleItem<Tuple::Foo3>(myTupleItemIdx, *this, modelPoint)
            {
            }

};


/** Concrete Read-Modify-Write Controller CONTAINER Point: BAR3
 */
template <class CONTEXT>
class RwmFoo3ControllerBar3: public Rte::Point::Controller::RmwContainer<Tuple::Foo3, CONTEXT>
{
public:
    /// Constructor. A child class must be create that implements Rte::Point::Controller::RmwClient::modify()
    RwmFoo3ControllerBar3( CONTEXT&                                                                          context, 
                           typename Rte::Point::Controller::RmwContainer<Tuple::Foo3, CONTEXT>::ModifyFunc_T modifyCallback, 
                           ModelBar3&                                                                        modelPoint )
        :Rte::Point::Controller::RmwContainer<Tuple::Foo3, CONTEXT>(context, modifyCallback, modelPoint )
            {}
};


};
#endif 