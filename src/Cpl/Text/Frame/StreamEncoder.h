#ifndef Cpl_Text_Frame_StreamEncoder_h_
#define Cpl_Text_Frame_StreamEncoder_h_
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


#include "Cpl/Text/Frame/Encoder.h"
#include "Cpl/Io/Output.h"



///
namespace Cpl { namespace Text { namespace Frame {



/** This concrete class implents the Encoder API where the Output destination
    is a Cpl::Io::Output stream.  There is no checking/enforcement of the
    content of the Frame (e.g. it will accept non-ASCII character) except
    for the SOF, EOF, and ESC characters.
 */
class StreamEncoder: public Encoder
{
protected:
    /// Output stream
    Cpl::Io::Output&    m_dst;

    /// Output/Frame state
    bool                m_inFrame;

    /// SOF character
    const char          m_sof;

    /// EOF character
    const char          m_eof;

    /// Escape character
    const char          m_esc;

    /// Remember my newline option
    const bool          m_appendNewline;



public:
    /** Constructor.  The 'appendNewline' argument when set to true will
        append a newline character to the output stream AFTER the EOF
        character (this can make for more human readable output stream).
     */
    StreamEncoder( Cpl::Io::Output& dst, char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline=true );


public:
    /// See Cpl::Text::Frame::Encoder
    bool startFrame( void ) throw();

    /// See Cpl::Text::Frame::Encoder
    bool output( char src ) throw();

    /// See Cpl::Text::Frame::Encoder
    bool output( const char* src ) throw();

    /// See Cpl::Text::Frame::Encoder
    bool output( const char* src, size_t numBytes ) throw();

    /// See Cpl::Text::Frame::Encoder
    bool endFrame( void ) throw();

};




};      // end namespaces
};
};
#endif  // end header latch
