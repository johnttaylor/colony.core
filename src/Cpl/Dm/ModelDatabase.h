#ifndef Cpl_Rte_ModelDatabase_h_
#define Cpl_Rte_ModelDatabase_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "Cpl/Dm/ModelDatabaseApi.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/Container/Map.h"


/** This symbol defines the size, in bytes, of a single/global parse buffer
    that is used for the fromString() operations so that the input strings
    can be parsed without directly updating a model point's data. Only one 
    instance of this buffer is allocated.
 */
#ifndef OPTION_CPL_RTE_MODEL_DATABASE_MAX_LENGTH_FROM_STRING_BUFFER
#define OPTION_CPL_RTE_MODEL_DATABASE_MAX_LENGTH_FROM_STRING_BUFFER      1024
#endif

///
namespace Cpl {
///
namespace Dm {


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
    /// See Cpl::Dm::ModelDatabaseApi
    ModelPoint * lookupModelPoint( const char* modelPointName ) const throw();
    
    /// See Cpl::Dm::ModelDatabaseApi
    ModelPoint* getFirstByName() const throw();

    /// See Cpl::Dm::ModelDatabaseApi
    ModelPoint* getNextByName( ModelPoint& currentModelPoint ) const throw();

public:
    /** This method has 'PACKAGE Scope' in that is should only be called by 
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid 
        the tight coupling of C++ friend mechanism.

        This method locks the Model Database.  For every call to lock() there must
        be corresponding call to unlock();
    */
    virtual void lock_() throw() { lock(); };

    /** This method has 'PACKAGE Scope' in that is should only be called by 
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid 
        the tight coupling of C++ friend mechanism.

        This method unlocks the Model Database.
    */
    virtual void unlock_() throw() { unlock(); }



public:
    /** This method has 'PACKAGE Scope' in that is should only be called by 
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid 
        the tight coupling of C++ friend mechanism.

        This method provides a single global lock for ALL Model Database 
        instances. The method is used to protect global Model Database (e.g.
        the global parse buffer). 
        
        This method locks the global Model Database lock. For every call to 
        globalLock_() there must be corresponding call to globalUnlock_();
    */
    static void globalLock_() throw();

    /** This method has 'PACKAGE Scope' in that is should only be called by 
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid 
        the tight coupling of C++ friend mechanism.

        This method unlocks the global Model Database lock
    */
    static void globalUnlock_() throw();

    /** This variable has 'PACKAGE Scope' in that is should only be called by 
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid 
        the tight coupling of C++ friend mechanism.
        
        Global/single instance of the fromString() parse buffer. Model Point's
        need to have acquired the global lock before using this buffer
     */
    static char g_parseBuffer_[OPTION_CPL_RTE_MODEL_DATABASE_MAX_LENGTH_FROM_STRING_BUFFER];

};


};      // end namespaces
};
#endif  // end header latch
