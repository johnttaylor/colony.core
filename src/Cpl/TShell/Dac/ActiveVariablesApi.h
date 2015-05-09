#ifndef Cpl_TShell_Dac_ActiveVariablesApi_h_
#define Cpl_TShell_Dac_ActiveVariablesApi_h_
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

#include "Cpl/TShell/Dac/VariableApi.h"
#include "Cpl/Text/String.h"


///
namespace Cpl { namespace TShell { namespace Dac {

/** This class defines the interface for managing a collection of in-use and/or 
    active DAC shell variables.
 */
class ActiveVariablesApi
{
public:
    /** This method searches - by variable name = the collection of active 
        variables and returns references to specified variable.  If the 
        variable specified is NOT current in-use/active - then a new
        variable is 'created' (using the specified name) and returned as well
        as being placed into the collection of in-use/active variables.  If
        the command can NOT be 'created' a new variable (e.g. out-of-memory)
        than 0 is returned. 
     */
    virtual VariableApi* get( const Cpl::Container::Key& variableName ) throw() = 0;


    /** This method searches - by variable name = the collection of active 
        variables and returns references to specified variable. If the 
        specified variable is not in the in-use/active collection then
        0 is returned.  This method is similiar to get() EXCEPT it does
        NOT attempt to create a new variable on the 'not-found' condition.
     */
    virtual VariableApi* find( const Cpl::Container::Key& variableName ) throw() = 0;


    /** This method is used to remove (i.e. 'destory') a variable from
        the active collection/state.
     */
    virtual void remove( VariableApi& varNoLongerInUse ) throw() = 0;
     

    /** This method returns the current number of active User variables
     */
    virtual unsigned getUserCount() const throw() = 0;
    

    /** This method returns the maximum number of active User variables allowed 
        and/or supported.
     */
    virtual unsigned getMaxUserCount() const throw() = 0;


public:
    /** This method add a 'system' variable to the list of active varaibles.
        ALL system variables must start with a unique (relative to 'user'
        variables) prefix, e.g. leading underscore: '_errno'. System 
        variables cannot be remove once added.
     */
    virtual void addSystem( VariableApi& systemVariable ) throw() = 0;


public:
    /// Returns the first active Variable in the collection.  Returns 0 if the collection is empty
    virtual VariableApi* first() const throw() = 0;


    /// Returns the nextactive Variable in the collection.  Returns 0 if there are no more variables in the collection
    virtual VariableApi* next( VariableApi& current ) const throw() = 0;


public:
    /// Virtual destructor
    virtual ~ActiveVariablesApi(){}

};

};      // end namespaces
};
};
#endif  // end header latch
