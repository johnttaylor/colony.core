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

#include "Cpl/Log/Loggers.h"
#include "Cpl/Log/Null.h"


/* This file provides default implementation of the Cpl::Log::Loggers 
   interface.  It targeted for platforms such as Windows, Linux, etc.
*/


using namespace Cpl::Log;


// Null logger
Null nullLogger_;


///////////////////////
Api& Loggers::application()
    {
    return nullLogger_;  // TODO: Replace with File logger, level:= Info
    }

Api& Loggers::console()
    {
    return nullLogger_;  // TODO: Replace with Stdout logger, level:= Info 
    }

Api& Loggers::fast()
    {
    return nullLogger_;  // TODO: Replace with File logger, level:= "Warning" 
    }

Api& Loggers::null()
    {
    return nullLogger_; 
    }

Api& Loggers::history()
    {
    return nullLogger_;  // TODO: Replace with File logger, level:= Info
    }





