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

#include "DHashTable_.h"


///
using namespace Cpl::Container;


//////////////////////////////////////////////////////////////////////////////

DHashTable_::DHashTable_( DList<DictItem> buckets[],
                          unsigned int    numBuckets,
                          HashFunc        func )
    : m_buckets( buckets )
    , m_numBuckets( numBuckets )
    , m_hashFunc( func )
    , m_numItems( 0 )
{
}


///////////////////////////////
void DHashTable_::insert( DictItem& node )
{
    // Plant the node in the indexed bucket.  Note: There is NO difference in
    // the handling of an empty-bucket vs. collision scenario since each
    // bucket IS a list.
    unsigned     keylength = 0;
    const void*  keyptr    = node.getKey().getRawKey( &keylength );
    unsigned int index     = m_hashFunc( keyptr, keylength, m_numBuckets );
    node.m_hashCode_       = index;
    m_buckets[index].put( node );
    m_numItems++;
}

DictItem* DHashTable_::find( const Key& keyToFind ) const
{
    // Hash the key
    unsigned         keylength = 0;
    const void*      keyptr    = keyToFind.getRawKey( &keylength );
    unsigned int     index     = m_hashFunc( keyptr, keylength, m_numBuckets );
    DList<DictItem>& list      = m_buckets[index];
    DictItem*        itemPtr   = list.first();

    // Traverse the hash bucket's list looking for a match
    while ( itemPtr )
    {
        if ( itemPtr->getKey().compareKey( keyToFind ) == 0 )
        {
            break;
        }
        itemPtr = list.next( *itemPtr );
    }

    // Return the matching item (or 0 if not one found)
    return itemPtr;
}

DictItem* DHashTable_::removeItem( DictItem& nodeToDelete )
{
    // Remove the node
    unsigned int index = nodeToDelete.m_hashCode_;
    if ( m_buckets[index].remove( nodeToDelete ) )
    {
        m_numItems--;
        return &nodeToDelete;
    }

    return 0;
}

DictItem* DHashTable_::first() const
{
    // Trap empty table
    if ( m_numItems )
    {
        // Find the first non-empty bucket in the table
        unsigned int i;
        for ( i = 0; i < m_numBuckets; i++ )
        {
            DictItem* itemPtr = m_buckets[i].first();
            if ( itemPtr )
            {
                return itemPtr;
            }
        }
    }

    // Empty table -->return null
    return 0;
}

DictItem* DHashTable_::next( DictItem& current ) const
{
    // Case: There are more item(s) in the current bucket
    unsigned int index   = current.m_hashCode_;
    DictItem*    nextPtr = m_buckets[index].next( current );
    if ( nextPtr )
    {
        return nextPtr;
    }

    // Case: Move to the next non-empty bucket
    while ( ++index < m_numBuckets )
    {
        if ( ( nextPtr = m_buckets[index].first() ) )
        {
            return nextPtr;
        }
    }

    // If I get here, then there are no more items in the table
    return 0;
}

//////////////////////////////
void DHashTable_::tableStats( HashTableStats& info ) const
{
    // Initialize stats structure
    info.m_numItems               = m_numItems;
    info.m_numBuckets             = m_numBuckets;
    info.m_numEmptyBuckets        = m_numBuckets;
    info.m_average_itemsPerBucket = 0.0;
    info.m_max_itemsPerBucket     = 0;
    if ( !m_numItems )
    {
        return;
    }

    // Walk all buckets
    unsigned int  i;
    unsigned int  filledBuckets = 0;
    unsigned long sumItemCount  = 0;
    for ( i = 0; i < m_numBuckets; i++ )
    {
        DictItem* itemPtr = m_buckets[i].first();
        if ( itemPtr )
        {
            info.m_numEmptyBuckets--;
            filledBuckets++;
            unsigned long itemCount = 0;
            while ( itemPtr )
            {
                itemCount++;
                itemPtr = m_buckets[i].next( *itemPtr );
            }
            sumItemCount += itemCount;
            if ( itemCount > info.m_max_itemsPerBucket )
            {
                info.m_max_itemsPerBucket = itemCount;
            }
        }
    }

    // Calc average
    info.m_average_itemsPerBucket = ( (double)sumItemCount ) / ( (double)filledBuckets );
}