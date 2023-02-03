/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2023 Michael N. Moran
*
* Redistributions of the source code must retain the above copyright notice.
*
* This implementation is an adaptation/simplification of the OSCL
* Oscl::Buffer::Ring ring buffer code.
*
* The adaptation was done by myself (Michael N. Moran) and donated to
* the Colony.Core Project.
*----------------------------------------------------------------------------*/

#ifndef Cpl_Container_Ring_h_
#define Cpl_Container_Ring_h_

#include <stdint.h>

/** */
namespace Cpl {
/** */
namespace Container {
/** */
namespace Ring {

/** This data structure represents the format of the data
    in the memory shared by the ring transmitter and the
    ring receiver (which may be the same in a single memory
    space system.)
    For multi-processor or shared memory applications that communicate
    through shared memory, this object *must* live in the shared memory.
 */
struct Header
{
    /** The variable indicates the number of elements in the
        ring buffer. The definition of the element itself is
        outside of the scope of this class.
     */
    const uint32_t      _nElements;

    /** This variable is updated only by the ring buffer
        receiver when an element is removed from the ring buffer.
     */
    volatile uint32_t   _readIndex;

    /** This variable is updated only by the ring buffer
        transmitter when an element is addded to the ring
        buffer.
     */
    volatile uint32_t   _writeIndex;

    // 
    Header(uint32_t nElements) noexcept:
        _nElements(nElements),
        _readIndex(0),
        _writeIndex(0)
        { }

    /** This operation returns true if the ring buffer
        is full and false otherwise.
     */
    bool    isFull() const noexcept
    {
        uint32_t
        nextIndex   = (_writeIndex + 1) % _nElements;

        return nextIndex == _readIndex;
    }

    /** This operation returns true if the ring buffer
        is empty and false otherwise.
     */
    bool isEmpty() const noexcept
    {
        return _readIndex == _writeIndex;
    }
};

/** This class is used by the entity that writes into the ring.
    In this implementation, the process of writing the ring buffer
    is divided into two stages. In the first stage, the client
    allocates a slot in the ring buffer. After allocating the slot,
    the client then fills the buffer at its convenience.
    In the second stage, after the buffer associated with the slot
    has been filled, the client then invokes a commit operation,
    which modifies the shared ring buffer announcing the availability
    of data to the receiving end.

    This object is accessed only by the transmitter, and may
    live in memory that is NOT shared with the receiver.

    NOTE:
        This class supports exactly one producer/transmitter.
        If multiple producers/transmitters need to access
        this instance, serialization/mutexes must be applied
        by the context.
 */
class TxDesc
{
    private:
        /** A variable used by the implementation to manage
            slot allocation.
         */
        uint32_t    _allocationOffset;

        /** This is a refrence to the ring descriptor that must
            live in the memory shared by the transmitter and
            receiver processes.
         */
        Header&     _header;

    public:
        /** The constructor requires a reference to the
            memory object shared with the receiver. It also
            can be invoked with a water mark value that
            determines the number of transmit buffers/slots
            that must be committed at startup before the
            shared descriptor is updated and they
            are actually made visible to the receiver.
         */
        TxDesc( Header& header ) noexcept;

        /** This operation attempts to allocate a writeable
            slot in the ring buffer. The index of the allocated
            slot is written to the variable referenced by the
            "index" argument if a slot is available.
            Returns true if a slot is available and false
            if there are no available slots.
         */
        bool    alloc( uint32_t& index ) noexcept;

        /** This operation returns true if the ring buffer
            is full and false otherwise.
         */
        bool    isFull() const noexcept;

        /** This operation is used to update the shared ring
            descriptor to indicate that a previously allocated
            slot (using the alloc() opration above) has been
            filled with data and is ready to be recognized by
            the receiving end of the ring.

            Note: Slots are assumed to be committed in the
            same order in which they are allocated.
         */
        void    commit() noexcept;

        /** This operation is used to reset the state.
            It is IMPORTANT that the consumer is aware
            of this. This awareness is the responsibility
            of the context.
         */
        void    reset() noexcept;
};

/** This class is used by the entity that reads the ring.
    In this implementation, the process of reading the ring buffer
    is divided into two stages. In the first stage, the client
    allocates a filled slot in the ring buffer. After a slot is
    allocated, the client then processes the data in the buffer
    at its convenience.

    Once the client finishes using the buffer associated with the
    allocated slot, the client then invokes a commit operation,
    which modifies the shared ring buffer announcing the free
    slot to transmitting end.

    This object is accessed only by the transmitter, and may
    live in memory that is NOT shared with the transmitter.

    NOTE:
        This class supports exactly one consumer/receiver.
        If multiple consumers/receivers need to access
        this instance, serialization/mutexes must be applied
        by the context.
 */
class RxDesc
{
    private:
        /** A variable used by the implementation to manage
            slot allocation.
         */
        uint32_t    _allocationOffset;

        /** This is a refrence to the ring descriptor that must
            live in the memory shared by the transmitter and
            receiver processes.
         */
        Header&     _header;

    public:
        /** The constructor requires a reference to the
            memory object shared with the receiver.
         */
        RxDesc( Header& header ) noexcept;

        /** This operation attempts to allocate a readable
            slot in the ring buffer. The index of the allocated
            slot is written to the variable referenced by the
            "index" argument if a slot is available.
            Returns true if a slot is available and false
            if there are no available slots.
         */
        bool    alloc( uint32_t& index ) noexcept;

        /** This operation resets the uncommitted allocation
            index. This allows readers to re-issue the
            alloc() operation which then returns the
            index of the first un-committed slot.
         */
        void    resetAllocationOffset() noexcept;

        /** This operation is used to update the shared ring
            descriptor to indicate that a previously allocated
            slot (using the alloc() opration above) has been
            released and is ready to be recognized as such by
            the transmitting end of the ring.

            Note: Slots are assumed to be committed in the
            same order in which they are allocated.
         */
        void    commit() noexcept;

        /** This operation is used to update the shared ring
            descriptor to indicate that all previously allocated
            slots (using the alloc() opration above) have been
            released and are ready to be recognized as such by
            the transmitting end of the ring.
         */
        void    commitAll() noexcept;

        /** This operation is used to reset the state.
            It is IMPORTANT that the producer is aware
            of this. This awareness is the responsibility
            of the context.
         */
        void    reset() noexcept;
};

/** This is a simple implementation of a ring buffer.
    NOTE:
        The producer and consumer operations are NOT reentrant.
        This means that they may be exactly one producer and
        one consumer.
        If multiple producers or consumers are required,
        then serialization/mutexes must be applied by the
        context.
 */
template <class ITEM, unsigned nItems>
class Simple
{
    private:
        /// The actual buffer where items are stored
        ITEM            _items[nItems];

        /// The indicies shared between the producer and consumer.
        Header          _header;

        /// The producer context
        TxDesc          _txdesc;

        /// The consumer context
        RxDesc          _rxdesc;

    public:
        /// Constructor
        Simple() noexcept:
            _header(nItems),
            _txdesc(_header),
            _rxdesc(_header)
            { }

    public:
        /** Removes the first item in the Buffer. The contents of the
            removed item will be copied into the 'dst' argument. The method
            return true if the operation was successful; else false is
            returned, i.e. the Ring buffer is/was empty.
        */
        bool remove( ITEM& dst ) noexcept
        {
            uint32_t    index;
            if( !_rxdesc.alloc(index) ){
                return false;
                }
            dst = _items[index];
            _rxdesc.commit();
            return true;
        }


        /** The contents of 'item' will be copied into the Ring Buffer as the
            'last' item in the  buffer. Return true if the operation was
            successful; else false is returned, i.e. the Buffer was full prior to
            the attempted add().
         */
        bool add( const ITEM& item ) noexcept
        {
            uint32_t    index;
            if( !_txdesc.alloc(index) )
            {
                return false;
            }
            _items[index]   = item;
            _txdesc.commit();
            return true;
        }

        /** Empties the Ring Buffer.  All references to the item(s) in the
            buffer are lost.

            It is IMPORTANT that both the producer and consumer are
            aware of this. Generally it means that both must be inactive
            when this operation is performed.
            This awareness is the responsibility of the context.
        */
        void clearTheBuffer() noexcept
        {
            _rxdesc.reset();
            _txdesc.reset();
            _header._readIndex    = 0;
            _header._writeIndex   = 0;
        }

        /** This method returns true if the Ring Buffer is empty
         */
        bool isEmpty( void ) const noexcept
        {
            return _header.isEmpty();
        }

        /** This method returns true if the Ring Buffer is full
         */
        bool isFull( void ) const noexcept
        {
            return _txdesc.isFull();
        }

};

}   // end namespaces
}
}

#endif  // end header latch
