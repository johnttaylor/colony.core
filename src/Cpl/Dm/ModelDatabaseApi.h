#ifndef Cpl_Rte_ModelDatabaseApi_h_
#define Cpl_Rte_ModelDatabaseApi_h_
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


///
namespace Cpl {
///
namespace Dm {

/// Forward reference to a Model point -->used to avoid circular dependencies
class ModelPoint;

/** This class defines the basic operations that can be performed on a Model
    Base.  A Model Database is a collection of instantiated Model Points.

    All methods in this class are thread Safe unless explicitly documented 
    otherwise.
 */
class ModelDatabaseApi
{
public:
    /** This method looks-up the model point instance by name and returns a
        pointer to the instance.  If the model point name cannot be found, the
        method returns 0.

        NOTE: The ModelPoint returned is a generic Model Point in that any
              operations that require the concrete data type of the Model 
              Point ARE NOT available.  It is 'okay' for the caller (IF AND ONLY
              IF the caller 'knows' what the concrete type is) to down cast the 
              returned pointer to a type specific child class instance of the 
              Model Point.

        NOTE: There is no 'add' method.  This is because Model Points self
              register with their assigned Model Database when the Model Points
              are created.
     */
    virtual ModelPoint* lookupModelPoint( const char* modelPointName ) const throw() = 0;


public:
    /** This method returns a pointer to the first Model Point in the Database.
        The model points are traversed in order by model point name.  If there
        are not Model Points in the Database, the method returns 0.
     */
    virtual ModelPoint* getFirstByName() const throw() = 0;

    /** This method returns the next (in sorted order) Model Point in the
        Database. If the current model point is the last Model Point in the
        Database the method returns 0.
     */
    virtual ModelPoint* getNextByName( ModelPoint& currentModelPoint ) const throw() = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~ModelDatabaseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
