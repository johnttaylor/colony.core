#ifndef Cpl_Rte_ModelBase_h_
#define Cpl_Rte_ModelBase_h_
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
namespace Cpl {
///
namespace Rte {


/** This class defines the basic operations that can be performed on a Model
    Base.  A Model base is a collection of instantiated Model Points.

    All methods in this class are thread Safe unless explicitly documented 
    otherwise.
 */
class ModelBase
{

public:
    /** This method has 'PACKAGE Scope' in that is should only be called by 
        other classes in the Cpl::Rte namespace.  It is ONLY public to avoid 
        the tight coupling of C++ friend mechanism.

        This method locks the Model Base.  For every call to lock() there must
        be corresponding call to unlock();
    */
    virtual void lock() throw() = 0;

    /** This method has 'PACKAGE Scope' in that is should only be called by 
        other classes in the Cpl::Rte namespace.  It is ONLY public to avoid 
        the tight coupling of C++ friend mechanism.

        This method unlocks the Model Base.
    */
    virtual void unlock() throw() = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~ModelBase() {}
};


};      // end namespaces
};
#endif  // end header latch
