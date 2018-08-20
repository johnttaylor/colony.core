#ifndef Cpl_Rte_Point_h_
#define Cpl_Rte_Point_h_
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

#include "Cpl/Text/String.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Rte {


/** This class defines the basic operations that can be performed on a Point.
    A Point represents the internal data/payload of a Model Point

    NOTES:
        1) The concrete leaf classes are responsible for ensuring type
           safety.
        2) All methods in this class are NOT thread Safe unless explicitly
           documented otherwise.
 */
class Point
{
public:
    /** This method returns the RAM size, in bytes, of the Point's data.
      */
    virtual size_t getSize() const throw() = 0;

    /** This method converts the Point's data to a a string value and copies
        the resultant string into 'dst'.  If the Point's data cannot be
        represented as a string then the contents of 'dst' is set to an empty
        string and the method returns false; else the method returns true. The
        format of the string is specific to the concrete leaf class.  However,
        it is strongly recommended that the output of this method be the same
        format that is expected for the fromString() method.

        NOTE: If the converted string is larger than the memory allocated by
              'dst' then the string result in 'dst' will be truncated. The
              caller is required to check the truncated() method of 'dst' to
              check for the truncated scenario.
     */
    virtual bool toString( Cpl::Text::String& dst, bool append=false ) const throw() = 0;

    /** This method attempts to convert the null terminated 'src' string to
        its binary format and copies the result to the Point's internal data.
        The expected format of the string is specific to the concrete leaf
        class.

        When 'terminationChars' is not 0, the conversion from text to binary is
        stopped if one of characters in 'terminationChars' is encountered.
        When 'terminationChars' is 0, the entire string contents (i.e. till
        '\0' is found) is converted.

        If the conversion is successful a pointer to next character after the
        last 'consumed' charactered is returned.  If the contents of the 'src'
        is invalid, OR the Point does not support a full/complete conversion
        from Text to binary, OR the conversion fails then the method returns 0.  
        When the conversion fails, the optional 'errorMsg' argument is updated 
        with a plain text error message.
     */
    virtual const char* fromString( const char* src, const char* terminationChars=0, Cpl::Text::String* errorMsg=0 ) throw() = 0;

    /** This method returns the maximum size, in bytes not including the null
        terminator, of the string returned by the toString() method.
     */
    virtual size_t getToStringMaxSize() throw() = 0;


public:
    /** This method is used to export THIS instance's data content
        to a raw data stream.  It is the responsibility of the caller to ensure
        that there is sufficient memory available for the data being exported.
        The method returns the number of bytes exported.
     */
    virtual size_t export(void* dstDataStream) const throw() = 0;

    /** This method is used to populate the data content from the a raw data
        stream/pointer.  It is the responsibility of the caller to ensure that
        the data stream is appropriate for element type and that the data
        stream content was originally created by the corresponding export()
        method. The method returns the number of bytes consumed from the data
        stream.
     */
    virtual size_t import( const void* srcDataStream ) throw() = 0;

    /** Returns the size, in bytes, of the element's data content.

        NOTE: The size returned is the size of the Point data WHEN it
              is being exported/imported - this is NOT the value of
              the size of the Point's internal storage use for the
              data content.
     */
    virtual size_t getExternalSize( void ) const = 0;
    

public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method is used to unconditionally update the Point's data.

        Notes:
        1) The assumption is that Point instance and 'src' are the of the same
           leaf class type.
        2) The internal data size of the Point instance is ALWAYS honored when
           coping the data from 'src'
        3) The Point's sequence number is not changed.
    */

    virtual void copyFrom_( const Point& src ) throw() = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Rte namespace.  The Application should
        NEVER call this method.

        This method compares the Point's data to the data of 'other' and
        returns true if the data of both points are the same. It is assumed
        that Point instance and 'other' are the of the same leaf class type.
     */
    virtual bool isEqual_( const Point& other ) const throw() = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~Point() {}
};


};      // end namespaces
};
#endif  // end header latch
