#ifndef Point_ControllerBar4_h_
#define Point_ControllerBar4_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

#include "Rte/Point/Controller/Base.h"
#include "Rte/Point/Controller/Tuple.h"
#include "Rte/Point/Controller/RmwComposer.h"
#include "Rte/TShell/Dac/_0test/Point/bar4_m.h"

///
namespace Point {


/** Concrete Controller Point: BAR1
 */
class ControllerBar4: public Point::Bar4,
                      public Rte::Point::Controller::Base
{
public:
    /// Constructor
    ControllerBar4( ModelBar4& modelPoint )
        :Rte::Point::Controller::Base(*this, modelPoint)
            {
            }

};


/** Concrete Tuple Controller Point: BAR1::Foo4
 */
class TupleFoo4ControllerBar4: public Tuple::Foo4, 
                               public Rte::Point::Controller::Tuple
{
public:
    /// Constructor
    TupleFoo4ControllerBar4( ModelBar4& modelPoint )
        :Rte::Point::Controller::Tuple(0, *this, modelPoint)
            {
            }

};


/** Concrete Read-Modify-Write Controller Point: BAR1
 */
template <class CONTEXT>
class RwmControllerBar4: public Point::Bar4,
                         public Rte::Point::Controller::RmwComposer<CONTEXT>
{
public:
    /// Constructor. A child class must be create that implements Rte::Point::Controller::RmwClient::modify()
    RwmControllerBar4( CONTEXT&                                                            context, 
                       typename Rte::Point::Controller::RmwComposer<CONTEXT>::ModifyFunc_T modifyCallback, 
                       ModelBar4&                                                          modelPoint )
        :Rte::Point::Controller::RmwComposer<CONTEXT>(*this, context, modifyCallback, modelPoint )
            {}
};





};
#endif 