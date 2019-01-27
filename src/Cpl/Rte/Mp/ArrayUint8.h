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


#include "Cpl/Rte/Mp/Basic.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Rte {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    Array of uint8_t.  Note: the memory for the array is dynamically allocated
    from the heap.  Once an instance is constructed, the memory/array size can
    NOT be changed.

    For the fromString() operation the expected data formats are:
    \code    

        <numElems>:<mpIndex>:<d0>:<d1>[:..]
        <numElems>:<mpIndex>:<h0><h1>[..]

        where:
            <numElems> is the number of <dN>/<hN> arguments
            <mpIndex>  is the starting Model Point element index to write the 
                       data to
            <eN>       is a/next source element.  The elements can be
                       decimal or hex depending on how the MP was constructed

        examples:
            3:7:32:33:34    -->writes the three values {32,33,34} to the
                               Model Point's array at indexes 7,8,9 respectively
            3:7:202122      -->same as above except that the element data is
                               in hex format.
    
    \endcode
    The toString() function outputs in the same data format as the expected 
    fromString() function.

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


protected:
    /// Decimal/Hex formating flat for toString()/fromString() operations
    bool m_decimal;


public:
    /** Constructor.  Invalid MP. 
     */
    ArrayUint8( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, bool decimalFormat = true);

    /** Constructor.  Valid MP.  Requires an initial value. If the 'srcData' 
        pointer is set to zero, then the entire array will be initialized to 
        zero.   Note: 'srcData' MUST contain at least 'numElements' elements.
     */
    ArrayUint8( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t numElements, const uint8_t* srcData, bool decimalFormat = true );

public:
    /** Type safe read. See Cpl::Rte::ModelPoint.

        The caller can read a subset of array starting from the specified index
        in the Model Point's array.  Note: if srcIndex + dstNumElements exceeds
        the size of the MP's data then the read operation will be truncated.
      */
    virtual uint16_t read( uint8_t* dstData, size_t dstNumElements, int8_t& validState, size_t srcIndex=0 ) const throw();

    /** Type safe write. See Cpl::Rte::ModelPoint.

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

    virtual uint16_t write( uint8_t* srcData, size_t srcNumElements, LockRequest_T lockRequest = eNO_REQUEST, size_t dstIndex=0 ) throw();


    /// Type safe read-modify-write client callback interface
    typedef Cpl::Rte::ModelPointRmwCallback<Data> Client;

    /** Type safe read-modify-write. See Cpl::Rte::ModelPoint

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
    typedef Cpl::Rte::Subscriber<ArrayUint8> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw();

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) throw();


public:
    ///  See Cpl::Rte::ModelPoint.
    bool toString( Cpl::Text::String& dst, bool append=false, uint16_t* retSequenceNumber=0 ) const throw();

    ///  See Cpl::Rte::ModelPoint.
    const char* getTypeAsText() const throw();

protected:
    /// See Cpl::Rte::ModelPointCommon_.
    const char* setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars=0, Cpl::Text::String* errorMsg=0, uint16_t* retSequenceNumber=0 ) throw();



};


};      // end namespaces
};
};
#endif  // end header latch
