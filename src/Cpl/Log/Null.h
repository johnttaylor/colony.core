#ifndef Cpl_Log_Null_h_
#define Cpl_Log_Null_h_
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

#include "Cpl/Log/Api.h"

           
///
namespace Cpl { namespace Log { 


/** This concrete class implements a "Null" logger, i.e. it discards all
    log messages.
 */
class Null: public Api
{
public:
    /// Constructor
    Null(){}


public:
    /// See Cpl::Log::Api
    void warning( const char* format, ... );

    /// See Cpl::Log::Api
    void info( const char* format, ... );

    /// See Cpl::Log::Api
    void verbose( const char* format, ... );
};


};      // end namespaces
};
#endif  // end header latch