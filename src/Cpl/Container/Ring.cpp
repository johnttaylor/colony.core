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
* This adaptation was done by myself (Michael N. Moran)
* and donated to the Colony.Core Project.
*----------------------------------------------------------------------------*/

#include "ring.h"

///
using namespace Cpl::Container::Ring;

TxDesc::TxDesc(
    Header&     header
    ) noexcept:
        _allocationOffset(0),
        _header(header)
{
}

bool    TxDesc::alloc(uint32_t& index) noexcept
{
    uint32_t
    nElements    = _header._nElements;

    uint32_t
    writeIndex    = _header._writeIndex + _allocationOffset;

    writeIndex    %=    nElements;

    uint32_t
    nextIndex    = (writeIndex + 1) % nElements;

    if( nextIndex == _header._readIndex )
    {
        return false;
    }
    else
    {
        index    = writeIndex;
        ++_allocationOffset;
        return true;
    }
}

bool    TxDesc::isFull() const noexcept
{
    uint32_t
    nElements    = _header._nElements;

    uint32_t
    index    = _header._writeIndex + _allocationOffset;

    index    %=    nElements;

    uint32_t
    nextIndex    =    (index + 1) % nElements;

    return nextIndex == _header._readIndex;
}

void    TxDesc::commit() noexcept
{
    if( !_allocationOffset )
    {
        /* This is a client error where no
           slot has been allocated before
           calling commit().
         */
        return;
    }

    uint32_t
    index    = _header._writeIndex;

    uint32_t
    nextIndex    = (index + 1) % _header._nElements;

    _header._writeIndex = nextIndex;

    --_allocationOffset;
}

void    TxDesc::reset() noexcept
{
    _allocationOffset   = 0;
}

RxDesc::RxDesc(
    Header& header
    ) noexcept:
        _allocationOffset(0),
        _header(header)
{
}

bool    RxDesc::alloc(uint32_t& index) noexcept
{
    uint32_t
    readIndex    = _header._readIndex + _allocationOffset;

    uint32_t
    nElements    = _header._nElements;

    readIndex        %= nElements;

    uint32_t
    writeIndex    = _header._writeIndex;

    if( readIndex == writeIndex )
    {
        return false;
    }
    else
    {
        ++_allocationOffset;
        index    = readIndex;
        return true;
    }
}

void    RxDesc::resetAllocationOffset() noexcept
{
    _allocationOffset    = 0;
}

void    RxDesc::commit() noexcept
{
    uint32_t
    nElements    = _header._nElements;

    uint32_t
    index        =    _header._readIndex + 1;
    index        %=    nElements;

    _header._readIndex  = index;

    --_allocationOffset;

}

void    RxDesc::commitAll() noexcept
{
    uint32_t
    nElements    = _header._nElements;

    uint32_t
    index        =    _header._readIndex + _allocationOffset;
    index        %=    nElements;

    _header._readIndex  = index;

    _allocationOffset    = 0;
}

void    RxDesc::reset() noexcept
{
    _allocationOffset   = 0;
}

