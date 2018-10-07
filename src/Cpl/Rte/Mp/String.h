#ifndef Cpl_Rte_Mp_String_h_
#define Cpl_Rte_Mp_String_h_
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

#include "Cpl/Rte/ModelPointCommon_.h"
#include "Cpl/Text/DFString.h"

/** This symbol defines the size, in bytes (not including the NULL terminator)
    of the input buffer that is used to contain results of parsing the 'input'
    for the fromString() method.  Only one instance of this buffer is allocated.
 */
#ifndef OPTION_CPL_RTE_MP_STRING_MAX_LENGTH_FROM_STRING_BUFFER
#define OPTION_CPL_RTE_MP_STRING_MAX_LENGTH_FROM_STRING_BUFFER      1024
#endif


///
namespace Cpl {
///
namespace Rte {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    null terminated string.  The storage for the internal string storage is
    done once when the instance is constructed, i.e. fixed length (per instance)
    storage.

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class String : public Cpl::Rte::ModelPointCommon_
{
protected:
    ///
    char*               m_data;
    ///
    size_t              m_maxLength;
    ///
    static char         g_buffer[OPTION_CPL_RTE_MP_STRING_MAX_LENGTH_FROM_STRING_BUFFER];

public:
    /// Constructor.  The 'maxLength' specifies the size, in bytes, of the string storage EXCLUDING the null terminator
    String( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t maxLength, const char* initialValue = "", int8_t validState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID );

public:
    /// Type safe read. See Cpl::Rte::ModelPoint
    virtual uint16_t read( Cpl::Text::String& dstData, int8_t& validState ) const throw();

    /// Type safe write. See Cpl::Rte::ModelPoint
    virtual uint16_t write( const char* srcData, LockRequest_T lockRequest = eNO_REQUEST ) throw();

    /// Same as write(), except only writes at most 'srcLen' bytes
    virtual uint16_t write( const char* srcData, size_t srcLen, LockRequest_T lockRequest = eNO_REQUEST ) throw();

    /// Type safe read-modify-write client callback interface
    typedef Cpl::Rte::ModelPointRmwCallback<Cpl::Text::String> Client;

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
    typedef Cpl::Rte::Subscriber<String> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw();

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) throw();


public:
    ///  See Cpl::Rte::ModelPoint.
    bool toString( Cpl::Text::String& dst, bool append=false, uint16_t* retSequenceNumber=0 ) const throw();

    ///  See Cpl::Rte::ModelPoint.
    const char* getTypeAsText() const throw();

    /// See Cpl::Rte::ModelPoint.  Note: the returned sized does INCLUDE the null terminator
    size_t getSize() const throw();


protected:
    /// See Cpl::Rte::ModelPointCommon_.
    const char* setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars=0, Cpl::Text::String* errorMsg=0, uint16_t* retSequenceNumber=0 ) throw();

    /// See Cpl::Rte::ModelPoint.  Note: Use the system wide default epsilon of CPL_MATH_REAL_FLOAT_EPSILON when testing for equality
    bool isDataEqual_( const void* otherData ) const throw();

    /// See Cpl::Rte::ModelPoint. Note: dstSize MUST include the null terminator
    void copyDataTo_( void* dstData, size_t dstSize ) const throw();

    /// See Cpl::Rte::ModelPoint.  Note: srcSize MUST include the null terminator
    void copyDataFrom_( const void* srcData, size_t srcSize ) throw();

    /// See Cpl::Rte::ModelPoint.  
    bool isDataEqual_( const void* otherData ) const throw();

    /// See Cpl::Rte::ModelPoint.  
    void* getDataPointer_() throw();

    /// See Cpl::Rte::ModelPoint.  
    size_t getInternalSize_() const throw();
};



};      // end namespaces
};
};
#endif  // end header latch
