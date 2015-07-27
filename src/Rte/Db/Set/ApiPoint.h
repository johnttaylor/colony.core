#ifndef Rte_Db_Set_ApiPoint_h_
#define Rte_Db_Set_ApiPoint_h_
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

#include "Rte/Point/Viewer/Api.h"
#include "Rte/Point/Controller/Api.h"
#include "Rte/Point/Model/Api.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Set { 


/** This class defines the interface that the final Concrete Set class
    is required to implement.  The interface provides the Set Layer access to 
    the concrete class's Viewer/Controlle/Point instances.
 */
class ApiPoint
{
public:
    /// Returns the reference to shared Point that is used by the Viewer and the Controller
    virtual Rte::Point::Api& getPoint(void) throw() = 0;

    /// Returns the reference to Concrete Set's viewer 
    virtual Rte::Point::Viewer::Api& getViewer() throw() = 0;

    /// Returns the reference to Concrete Set's controller 
    virtual Rte::Point::Controller::Api& getController() throw() = 0;

    /// Returns the reference to the Model Point being viewed/controlled
    virtual Rte::Point::Model::Api&  getModelPoint() throw() = 0;

    /** Sets the contents of the Controller to the Set's default value(s).
        Note: The method does NOT update the Model Point - just the Controller
              Point's local memory
     */
    virtual void defaultSet( void ) throw() = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~ApiPoint(void){}
};


};      // end namespaces
};
};
#endif  // end header latch
