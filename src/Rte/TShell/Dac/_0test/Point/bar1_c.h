#ifndef Point_ControllerBar1_h_
#define Point_ControllerBar1_h_
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

#include "Rte/Point/Controller/Base.h"
#include "Rte/Point/Controller/Tuple.h"
#include "Rte/Point/Controller/RmwComposer.h"
#include "Rte/TShell/Dac/_0test/Point/bar1_m.h"

///
namespace Point {


/** Concrete Controller Point: BAR1
 */
class ControllerBar1: public Point::Bar1,
                      public Rte::Point::Controller::Base
{
public:
    /// Constructor
    ControllerBar1( ModelBar1& modelPoint )
        :Rte::Point::Controller::Base(*this, modelPoint)
            {
            }

};


/** Concrete Tuple Controller Point: BAR1::Foo1
 */
class TupleFoo1ControllerBar1: public Tuple::Foo1, 
                               public Rte::Point::Controller::Tuple
{
public:
    /// Constructor
    TupleFoo1ControllerBar1( ModelBar1& modelPoint )
        :Rte::Point::Controller::Tuple(0, *this, modelPoint)
            {
            }

};


/** Concrete Read-Modify-Write Controller Point: BAR1
 */
template <class CONTEXT>
class RwmControllerBar1: public Point::Bar1,
                         public Rte::Point::Controller::RmwComposer<CONTEXT>
{
public:
    /// Constructor. A child class must be create that implements Rte::Point::Controller::RmwClient::modify()
    RwmControllerBar1( CONTEXT&                                                            context, 
                       typename Rte::Point::Controller::RmwComposer<CONTEXT>::ModifyFunc_T modifyCallback, 
                       ModelBar1&                                                          modelPoint )
        :Rte::Point::Controller::RmwComposer<CONTEXT>(*this, context, modifyCallback, modelPoint )
            {}
};





};
#endif 