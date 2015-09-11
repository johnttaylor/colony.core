#ifndef Rte_Point_Controller_Api_h_
#define Rte_Point_Controller_Api_h_
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


///
namespace Rte { namespace Point { namespace Controller {



/** This class defines the interface for the base operations of
    client Controller Point
 */
class Api
{
public:
    /** This method takes the controller point's current content
        and updates the associated model point. The method returns true
        when the update is successful; else false is returned  
     */
    virtual bool updateModel( void ) = 0;


public: 
    /// Virtual destructor to make the compiler happy
    virtual ~Api(void){}
};


};      // end namespaces
};
};
#endif  // end header latch
