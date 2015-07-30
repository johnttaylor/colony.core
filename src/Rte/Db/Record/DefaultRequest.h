#ifndef Rte_Db_Record_DefaultRequest_h_
#define Rte_Db_Record_DefaultRequest_h_
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
namespace Rte { namespace Db { namespace Record {


/** This abstact class define message types and payloads for a set of
    ITC services. This interface will can default the ENTIRE database
    OR it can default an individual record - what is default is determined
    by the actual service provider (i.e. if the the service provider is
    the Record::Handler - then the entire DB is defaulted.  If the service
    provider is a Record - then just that record is defaulted.
 */
class DefaultRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<DefaultRequest> SAP;


public:
    /// Payload for Message: Default the Record/All Records
    class DefaultPayload
    {
    public:
        /// Constructor
        DefaultPayload(){}

    };

    /// Message Type: Default the Record/All Records
    typedef Cpl::Itc::RequestMessage<DefaultRequest,DefaultPayload> DefaultMsg;


public:
    /// Request: Default the Record/All Records
    virtual void request( DefaultMsg& msg ) = 0;
    
};


};      // end namespaces
};
};
#endif  // end header latch
