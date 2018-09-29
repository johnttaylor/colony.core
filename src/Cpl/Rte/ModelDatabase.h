#ifndef Cpl_Rte_ModelDatabase_h_
#define Cpl_Rte_ModelDatabase_h_
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

#include "Cpl/Rte/ModelDatabaseApi.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/Container/Map.h"


///
namespace Cpl {
///
namespace Rte {


/** This concrete class implements a simple Model Database
 */
class ModelDatabase : 
    public ModelDatabaseApi, 
    public Cpl::System::Mutex, 
    public Cpl::Container::Map<ModelPoint>
{
public:
    /// Constructor
    ModelDatabase() throw();

    /** This is a special constructor for when the Model Database is 
	    statically declared (i.e. it is initialized as part of
	    C++ startup BEFORE main() is executed.  C/C++ does NOT guarantee
	    the order of static initializers.  Since the Model Database contains
        a list of model points - and one or more of those model points can
        be statically created/initialized - we need to do something to ensure
        my internal list is properly initialized for this scenario - hence this
        constructor.
	 */
	 ModelDatabase(const char* ignoreThisParameter_usedToCreateAUniqueConstructor) throw();

public:
    /// See Cpl::Rte::ModelDatabaseApi
    ModelPoint * lookupModelPoint( const char* modelPointName ) const throw();

public:
    /** This method has 'PACKAGE Scope' in that is should only be called by 
        other classes in the Cpl::Rte namespace.  It is ONLY public to avoid 
        the tight coupling of C++ friend mechanism.

        This method locks the Model Database.  For every call to lock() there must
        be corresponding call to unlock();
    */
    virtual void lock_() throw() { lock(); };

    /** This method has 'PACKAGE Scope' in that is should only be called by 
        other classes in the Cpl::Rte namespace.  It is ONLY public to avoid 
        the tight coupling of C++ friend mechanism.

        This method unlocks the Model Database.
    */
    virtual void unlock_() throw() { unlock(); }
};


};      // end namespaces
};
#endif  // end header latch
