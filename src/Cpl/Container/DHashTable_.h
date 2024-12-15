#ifndef Cpl_Container_DHashTable_x_h_
#define Cpl_Container_DHashTable_x_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Container/DictItem.h"
#include "Cpl/Container/Hash.h"
#include "Cpl/Container/HashFuncDefault.h"
#include "Cpl/Container/DList.h"


///
namespace Cpl {
///
namespace Container {


/** This concrete class implements the core functionality for a Dictionary
    and/or Hash Table. The class requires the application to supply the memory
    and hash function (a default function is available) for the hash table.
    
    Key collisions are handled by a simple link list for each 'hash bucket'. 
    Clients should NOT USE THIS CLASS DIRECTLY, but via the Dictionary class.  

    NOTE: This is no checking for duplicate keys. You can insert multiple
          items with duplicates keys, but there is no guaranty on how
          those items are found in searches or removed from the table.
 */
class DHashTable_
{
private:
    /// Array of hash buckets
    DList<DictItem>* m_buckets;

    /// Number of hash buckets
    unsigned int m_numBuckets;

    /// Pointer to the hashing function
    HashFunc m_hashFunc;

    /// Number of items in the table
    unsigned long m_numItems;

public:
    /// Constructor
    DHashTable_( DList<DictItem> buckets[], unsigned int numBuckets, HashFunc func = Cpl::Container::hashFuncDefault );


public:
    /** Inserts an item into the table.
     */
    void insert( DictItem& node );

    /** Removes the specified item from the table.  Returns the
        node removed.  If the remove fails (i.e. node does
        not exist in the table), then 0 is returned.
     */
    DictItem* removeItem( DictItem& nodeToDelete );

    /** Searches for a item with a matching key.  Returns the node that
        matches, else 0.
     */
    DictItem* find( const Key& keyToFind ) const;

    /// Returns the first item in the table. Returns 0 if table is empty
    DictItem* first() const;

    /// Returns the next item in the table.  Returns 0 if at the end-of-table
    DictItem* next( DictItem& current ) const;


public:
    /** Returns table stats.  Caller provides the memory for the stats structure.

        Note: The stats are not calculate/gathered until this method is called.
              The duration of this call is directly related to the number of
              items in the hash table.
     */
    void tableStats( HashTableStats& tableInfo ) const;
};


};  // end namespaces
};
#endif  // end header latch
