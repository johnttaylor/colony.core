#ifndef Cpl_Rte_Mp_ArrayUint8_h_
#define Cpl_Rte_Mp_ArrayUint8_h_
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


#include "Cpl/Dm/Mp/Basic.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    Array of uint8_t.  Note: the memory for the array is dynamically allocated
    from the heap.  Once an instance is constructed, the memory/array size can
    NOT be changed.

    The toJSON()/fromJSON format is:
        \code

        { name="<mpname>", type="<mptypestring>", invalid=nn, seqnum=nnnn, locked=true|false, val:{start:<idx>, elems:[<n0>,<n1>,...]} }

        where <idx> is the start index for the values in 'elems'. If <idx> is
        not specified, then it defaults to 0.

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class ArrayUint8 : public Array<uint8_t>
{
public:
    /** The MP's Data container.
        NOTE: The client(s) are RESPONSIBLE for honoring the numElements
              field.
     */
    typedef struct
    {
        uint8_t*  firstElemPtr; //!< Pointer to the first element in the array
        size_t    numElements;  //!< Number of elements in the array
    } Data;



public:
    /** Constructor.  Invalid MP.
     */
    ArrayUint8( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements );

    /** Constructor.  Valid MP.  Requires an initial value. If the 'srcData'
        pointer is set to zero, then the entire array will be initialized to
        zero.   Note: 'srcData' MUST contain at least 'numElements' elements.
     */
    ArrayUint8( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, const uint8_t* srcData );

public:
    /** Type safe read. See Cpl::Dm::ModelPoint.

        The caller can read a subset of array starting from the specified index
        in the Model Point's array.  Note: if srcIndex + dstNumElements exceeds
        the size of the MP's data then the read operation will be truncated.
      */
    virtual int8_t read( uint8_t* dstData, size_t dstNumElements, size_t srcIndex=0, uint16_t* seqNumPtr=0 ) const noexcept;

    /** Type safe write. See Cpl::Dm::ModelPoint.

        The caller can write a subset of array starting from the specified index
        in the Model Point's array.  Note: if dstIndex + srcNumElements exceeds
        the size of the MP's data then the write operation will be truncated

        NOTE: The application/caller is responsible for what a 'partial write'
              means to the integrity of the MP's data.  WARNING: Think before
              doing a partial write!  For example, if the MP is in the invalid
              state and a partial write is done - then the MP's data/array is
              only partially initialized AND then MP is now in the valid
              state!
      */

    virtual uint16_t write( uint8_t* srcData, size_t srcNumElements, LockRequest_T lockRequest = eNO_REQUEST, size_t dstIndex=0 ) noexcept;


    /// Type safe read-modify-write client callback interface
    typedef Cpl::Dm::ModelPointRmwCallback<Data> Client;

    /** Type safe read-modify-write. See Cpl::Dm::ModelPoint

       NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
             potential to lockout access to the ENTIRE Model Base for an
             indeterminate amount of time.  And alternative is to have the
             concrete Model Point leaf classes provide the application
             specific read, write, read-modify-write methods in addition or in
             lieu of the read/write methods in this interface.
     */
    virtual uint16_t readModifyWrite( Client& callbackClient, LockRequest_T lockRequest = eNO_REQUEST );


public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ArrayUint8> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) noexcept;


public:
    ///  See Cpl::Dm::ModelPoint.
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;
};


};      // end namespaces
};
};
#endif  // end header latch
