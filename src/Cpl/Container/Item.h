#ifndef Cpl_Container_Item_h_
#define Cpl_Container_Item_h_
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


/// 
namespace Cpl { namespace Container {



/** This class is used by the Container classes to implement a various types 
    of singly linked containers.

    Note: Client code, i.e. application code that needs to define a 
          'containerized' class only needs to inherit from this 
          interface.  The Client code SHOULD/SHALL NOT access any of its 
          members or methods!  These members/methods are intended to ONLY be
          accessible by the container classes.
 */
class Item
{
public:
    /// The link field.
    void*  m_nextPtr_;

    /** Debug field.  This member is used to trap when there is an attempt 
        to insert a item into a container when it is already in a container
     */
    void*  m_inListPtr_;

protected:
    /// Constructor
    Item():m_nextPtr_(0),m_inListPtr_(0){}



public:
    /** Helper method to trap when inserting an item in multiple containers.  
        A fatal error will be generated if 'Item' is attempted to be put into
        more than on container.

        Notes: 
            o Calling this method with 'newContainerP' set to zero will have
              the same effect as calling remove_().
            o The method returns false when there is error - which is ONLY
              usefully during unittesting when the invoking a FatalError
              does NOT terminate the application.
     */
    bool insert_(void* newContainerPtr);

    /** Returns 'true' if the instance is in the specified container.
     */
    bool isInContainer_( void* containerPtr );
  
  
    /** Helper method to do the proper 'clean-up' for the 
        multiple-containers-error-trap when removing an item from a container.
     */
    static void remove_(Item* itemPtr) throw();

};

/** This class is used by the Container classes
    to implement a various types of DOUBLY linked 
    containers.

    Note: Client code, i.e. application code that needs to define a 
          'containerized' class only needs to inherit from this 
          interface.  The Client code SHOULD/SHALL NOT access any of its 
          members or methods!  These members/methods are intended to ONLY be
          accessible by the container classes.
 */

class ExtendedItem: public Item 
{
public:
    /// The previous link field.
    void*   m_prevPtr_;

protected:
    /// Constructor
    ExtendedItem():m_prevPtr_(0){}

};


};      // end namespaces
};
#endif  // end header latch

