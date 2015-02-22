#ifndef Cpl_Log_Loggers_h_
#define Cpl_Log_Loggers_h_
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

#include "Cpl/Log/Api.h"

           
///
namespace Cpl { namespace Log {


/** This class provides a collection of pre-defined logger instances. The exact 
    behavior of the loggers below is DETERMINED BY THE APPLICATION and what it 
    platform supports. For example: An application can choose to use the 'Null' 
    logger for all loggers in the collection, which effectively disables all 
    Cpl::Log::Api logging 
 */
class Loggers
{
public:
    /** This method returns a reference to the logger instance that is the
        default logger (as determined by the application).
     */
    static Api& application();

    /** This method returns a reference to a logger instance that routes all
        log messages to the platform's 'stdout' interface
     */
    static Api& console();

    /** This method returns a reference to a logger instance that discards all
        log messages.
     */
    static Api& null();

    /** This method returns a reference to a logger instance tha emphasizes 
        speed of the logging over content, e.g. this logger may dropper lower 
        importance log messages for performance reasons. 
     */
    static Api& fast();

    /** This method returns a reference to a logger instance that records the log 
        messages to some form of non-volatile memory     
     */
    static Api& history();
};


};      // end namespaces
};
#endif  // end header latch





