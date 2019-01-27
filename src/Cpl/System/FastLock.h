#ifndef Cpl_System_FastLock_h_
#define Cpl_System_FastLock_h_
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

#include "colony_map.h"

/// Defer the definition of the a raw fast-lock type to the application's 'platform'
#define Cpl_System_FastLock_T          Cpl_System_FastLock_T_MAP


///
namespace Cpl {
///
namespace System {

/** This class defines an interface for a "Fast Lock".  A Fast Lock provides
    mutual exclusion and/or a critical section protection similar to a Mutex
    except that is it intended to be lighter weight and/or have faster
    performance than a traditional Mutex.  However, the performance gain comes
    with the  following constraints:

        o Non recursive semantics.  The calling thread CANNOT attempt to acquire
          the lock a second time once it has already acquire the lock.

        o The code that is protected by this lock MUST BE VERY SHORT time
          wise and NOT call an operating system methods.

    Why the above the constraints?  The implementation of a FastLock is
    target platform specific and the implementation can vary from disable/enable
    interrupts on a RTOS to spin-locks in user space.  So, use FastLocks with
    care and always honor the above constraints.
 */
class FastLock
{
public:
    /// Constructor
    FastLock();

    /// Destructor
    ~FastLock();

public:
    /** This method is invoked prior to entering a critical
        section.  If another thread currently "owns" the
        mutex, the current thread will "wait" until it
        can obtain ownership before proceeding.
     */
    void lock( void );

    /** This method is invoke at the end of a critical
        section.  This call will release the ownership of
        the lock.
     */
    void unlock( void );


protected:
    /// Raw FastLock handle/instance/pointer
    Cpl_System_FastLock_T  m_flock;


private:
    /// Prevent access to the copy constructor -->mutexes can not be copied!
    FastLock( const FastLock& m );

    /// Prevent access to the assignment operator -->mutexes can not be copied!
    const FastLock& operator=( const FastLock& m );

};



};      // end namespaces
};
#endif  // end header latch
