#ifndef Cpl_Text_Frame_Encoder_x_h_
#define Cpl_Text_Frame_Encoder_x_h_
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



///
namespace Cpl { namespace Text { namespace Frame {



/** This Private Namespace partially concrete class implents common/helper 
    method for supporting the Encoder API. There is no checking/enforcement 
    of the content of the Frame (e.g. it will accept non-ASCII character) 
    except for the SOF, EOF, and ESC characters.
 */
class Encoder_: public Encoder
{
protected:
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



protected:
    /** Constructor.  The 'appendNewline' argument when set to true will
        append a newline character to the output stream AFTER the EOF
        character (this can make for more human readable output stream).

        NOTE: If 'startOfFrame' is '\0', then Encoder will begin the
              frame WITHOUT inserting a SOF chracter.  This is useful
              when there application desires/has multiple start-of-frame
              characters for a given frame.
     */
    Encoder_( char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline=true );


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


protected:
    /// Helper method - implemented by the child class
    virtual bool start( char src ) throw() = 0;
    
    /// Helper method - implemented by the child class
    virtual bool start() throw() = 0;

    /// Helper method - implemented by the child class
    virtual bool append( char src ) throw() = 0;
};




};      // end namespaces
};
};
#endif  // end header latch
