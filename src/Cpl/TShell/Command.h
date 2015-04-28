#ifndef Cpl_TShell_Command_h_
#define Cpl_TShell_Command_h_
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

#include "Cpl/Container/MapItem.h"


///
namespace Cpl { namespace TShell {

// Forward name referencing to avoid circular header file dependencies
class Processor;


/** This class defines the interface for a TShell command.
 */

class Command: public Cpl::Container::MapItem
{
public:         
    /// Possible result codes when executing a command
    enum Result_T { eSUCCESS=0,             //!< Command executed successfully
                    eERROR_IO,              //!< Command failed due to an error writing to the Output stream
                    eERROR_MISSING_ARGS,    //!< Command failed due to execute due to missing one or more required arguments
                    eERROR_INVALID_ARGS,    //!< Command failed due to execute due to one or more arguments being invalid
                    eERROR_FAILED           //!< Command failed to complete all of its actions
                    eERROR_PARTIAL          //!< Command completed some of it actions, but not all and the uncompleted work is not consider a 'hard' failure.
                  }; 



public:
    /** This method executes the command
     */
    virtual Result_T execute( Processor& context, char* inputString ) throw() = 0;


public:
    /// This method returns the command's verb string
    virtual const char* getVerb() const throw() = 0;


    /// This method returns the command's usage string
    virtual const char* getUsage() const throw() = 0;


    /** This method returns the command's detailed help string.  Detailed
        help is optional.  If the command does not support detailed help,
        then 0 is returned.
     */
    virtual const char* getHelp() const throw() = 0;


public:
    /// Virtual destructor
    virtual ~Command(){}

};

};      // end namespaces
};
#endif  // end header latch
