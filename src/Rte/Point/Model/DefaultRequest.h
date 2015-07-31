#ifndef Rte_Point_Model_DefaultRequest_h_
#define Rte_Point_Model_DefaultRequest_h_
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


#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/SAP.h"


///
namespace Rte { namespace Point { namespace Model {


/** This abstact class define message types and payloads for the set of
    ITC services used have the Model Point change/update its data to
    its 'default values' (as defined by the application)
 */
class DefaultRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<DefaultRequest> SAP;


public:
    /// Payload for Message: Default the Model/All Models
    class DefaultPayload
    {
    public:
        /// Constructor
        DefaultPayload(){}

    };

    /// Message Type: Default the Model/All Models
    typedef Cpl::Itc::RequestMessage<DefaultRequest,DefaultPayload> DefaultMsg;


public:
    /// Request: Default the Model/All Models
    virtual void request( DefaultMsg& msg ) = 0;
    
};


};      // end namespaces
};
};
#endif  // end header latch
