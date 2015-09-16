#ifndef Cpl_Text_Frame_StringDecoder_h_
#define Cpl_Text_Frame_StringDecoder_h_
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


#include "Cpl/Text/Frame/Decoder_.h"
#include "Cpl/Text/String.h"


///
namespace Cpl { namespace Text { namespace Frame {



/** This concrete class defines an interface a Text "Decoder" that 
    accepts a null terminated string as its inout source. See 
    Cpl::Text::Frame::Decoder for details on what is a decoder. 
 */
class StringDecoder: public Decoder_
{
protected:
    /// Input source
    const char*        m_srcPtr;

    /// Input source length
    int                m_srcLen;


public:
    /** Constructor. The optional 'inputSource' argument points to the raw 
        frame data to be decoded.  If 'inputSource' is not set at construction 
        time, it MUST be set BEFORE scan() is called or a fatal error will be 
        generated.

        NOTE: Once scan() has be called the 'inputSouce' is considered to be 
              consumed and the Application must make a call to setInput()
              before calling scan() again (if not, the scan() call will fail
              - i.e. return false - with an end-of-input error).
     */
    StringDecoder( const char* inputSourceAsNullTerminatedString = 0 );



public:
    /** This method allows the Application/consumer to change/Set the Input
        source.
     */
    void setInput( const char* inputSourceAsNullTerminatedString ) throw();

    /** This method allows the Application/consumer to change/Set the Input
        source.  Note: 'sizeInBytesOfSource' does NOT include the/a null 
        terminator
     */
    void setInput( const char* inputSoruce, int sizeInBytesOfSource ) throw();



protected:
    /// See Cpl::Text::Frame::Decoder_
    bool read( void* buffer, int numBytes, int& bytesRead );

};




};      // end namespaces
};
};
#endif  // end header latch
