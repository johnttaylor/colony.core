#ifndef Cpl_TShell_Debug_ActiveVariables_h_
#define Cpl_TShell_Debug_ActiveVariables_h_
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

#include "Cpl/THsell/Debug/Variable_.h"


///
namespace Cpl { namespace TShell { namespace Debug {

/** This Private Namesapce class defines the interface managing a
    collection of in-use and/or active Debug shell variables.

    NOTE: This class IS-A a Container Map, so clients can walk the 
          set of active Varaibles using the Cpl::Container::Map 
          interface.  Clients SHOULD NEVER use the Map interface
          for adding to the set of active Variables!
 */

class ActiveVariables_
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
    virtual Variable_* get( const char* variableName ) throw() = 0;


    /** This method searches - by variable name = the collection of active 
        variables and returns references to specified variable. If the 
        specified variable is not in the in-use/active collection then
        0 is returned.  This method is similiar to get() EXCEPT it does
        NOT attempt to create a new variable on the 'not-found' condition.
     */
    virtual Variable_* find( const char* variableName ) throw() = 0;


    /** This method is used to remove (i.e. 'destory') a variable from
        the active collection/state.
     */
    virtual void remove( Variable_& varNoLongerInUse ) throw() = 0;


public:
    /// Returns the first active Variable in the collection.  Returns 0 if the collection is empty
    Variable_* first() const;

    /// Returns the nextactive Variable in the collection.  Returns 0 if there are no more variables in the collection
    Variable_* next( Variable_& current ) const;


public:
    /// Virtual destructor
    virtual ~ActiveVariables_(){}

};

};      // end namespaces
};
};
#endif  // end header latch
