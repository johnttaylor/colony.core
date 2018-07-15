#ifndef Cpl_Log_Api_h_
#define Cpl_Log_Api_h_
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


           
///
namespace Cpl { namespace Log { 

/** This abstract class defines an interface for logging events and/or
    application information at run time in a released application.  Where,
    when, format, additional information, and how the logged information 
    is stored/routed/displayed/etc, is determined by the child classes 
    that implement this interface.  

    The interface provides severity levels when logging information. It is up 
    to the implementation class to determine how to handle/process the severity 
    level, but the intent of the severity levels is it is a hierarchical ranking 
    of the importance of the information being  logged.

    Logging Levels:

        Warning         -->Highest importance,
        Information,
        Verbose         -->Least importance,


    NOTE: See the Cpl::System::Trace interface for a debugging, printf style
          trace logging functionality.
 */

class Api
{
public:
    /** This method logs the application's information with a severity
        level of "Warning".  The content of the log message is created 
        using printf() semantics.
     */
    virtual void warning( const char* format, ... ) = 0;

    /** This method logs the application's information with a severity
        level of "Information". The content of the log message is created 
        using printf() semantics.
     */
    virtual void info( const char* format, ... ) = 0;

    /** This method logs the application's information with a severity
        level of "Verbose". The content of the log message is created 
        using printf() semantics.
     */
    virtual void verbose( const char* format, ... ) = 0;


public:
    /// Virtual destructor
    virtual ~Api(){}

};


};      // end namespaces
};
#endif  // end header latch