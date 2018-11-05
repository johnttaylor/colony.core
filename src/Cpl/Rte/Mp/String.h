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



 ///
namespace Cpl {
///
namespace Rte {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    null terminated string.  The storage for the internal string storage is
    allocated (from the heap) ONCE when the instance is constructed, i.e. fixed 
    length (per instance) storage.

    For the fromString() operation the expected data formats are:
        <simple-text>
        "<text-string>"

        where:
            <simple-text> is a string containing no whitespace AND does NOT
                          contain any 'special characters' with respect to the
                          application's source stream.
            <text-string> is a string bounded with double quotes and can contain
                          any printable character.  The Cpl::Text::Frame::StringDecoder
                          class is used with the framing/special characters 
                          defined in the Cpl/Rte/ModelPoint.h header file


        examples:
            noWhitespace 
            "I have white space and `"quote`" characters"
    
    The toString() function outputs in the same data format as the expected 
    fromString() function - except the toString() always output the string
    as a <text-string>

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class String : public Cpl::Rte::ModelPointCommon_
{
public:
    /** The MP's Data container.
        NOTE: The client(s) are RESPONSIBLE for honoring the max string length
              and ensuring that the resultant string is properly null
              terminated and the stringLen field is set correctly
     */
    typedef struct
    {
        char*  stringPtr;       //!< Pointer to the string data.  MUST ALWAYS point to a null terminated string!
        size_t stringLen;       //!< Length, in bytes, of the string data.  Does NOT include the null terminator.
        size_t maxLength;       //!< Maximum length, in bytes, for the string data NOT including the null terminator
    } Data;

protected:
    /// Storage for the MP's data
    Data                m_data;

public:
    /// Constructor.  Invalid MP. The 'maxLength' specifies the size, in bytes, of the string storage EXCLUDING the null terminator
    String( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t maxLength );

    /// Constructor.  Valid MP. Requires an initial value. The 'maxLength' specifies the size, in bytes, of the string storage EXCLUDING the null terminator
    String( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t maxLength, const char* initialValue  );

    /// Destructor (free up allocate array memory)
    ~String();

public:
    /// Type safe read. See Cpl::Rte::ModelPoint
    virtual uint16_t read( Data& dstData, int8_t& validState ) const throw();

    /// Type safe read. See Cpl::Rte::ModelPoint
    virtual uint16_t read( Cpl::Text::String& dstData, int8_t& validState ) const throw();


    /// Type safe write. See Cpl::Rte::ModelPoint
    virtual uint16_t write( const Data& srcData, LockRequest_T lockRequest = eNO_REQUEST ) throw();

    /// Type safe write of a null terminated string. See Cpl::Rte::ModelPoint
    virtual uint16_t write( const char* srcData, LockRequest_T lockRequest = eNO_REQUEST ) throw();

    /// Same as write(), except only writes at most 'srcLen' bytes
    virtual uint16_t write( const char* srcData, size_t srcLen, LockRequest_T lockRequest = eNO_REQUEST ) throw();

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

    /// See Cpl::Rte::ModelPoint.  Note: the returned sized does DOES NOT the null terminator
    size_t getSize() const throw();


protected:
    /// See Cpl::Rte::ModelPointCommon_.
    const char* setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars=0, Cpl::Text::String* errorMsg=0, uint16_t* retSequenceNumber=0 ) throw();

    /// See Cpl::Rte::ModelPoint. Note: dstSize DOES NOT include the null terminator
    void copyDataTo_( void* dstData, size_t dstSize ) const throw();

    /// See Cpl::Rte::ModelPoint.  Note: srcSize DOES NOT include the null terminator
    void copyDataFrom_( const void* srcData, size_t srcSize ) throw();

    /// See Cpl::Rte::ModelPoint.  
    bool isDataEqual_( const void* otherData ) const throw();

    /// See Cpl::Rte::ModelPoint.  
    const void* getImportExportDataPointer_() const throw();

    /// See Cpl::Rte::ModelPoint.  
    size_t getImportExportSize_() const throw();
};



};      // end namespaces
};
};
#endif  // end header latch
