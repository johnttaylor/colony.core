#ifndef Cpl_Text_Frame_AsciiDecoder_h_
#define Cpl_Text_Frame_AsciiDecoder_h_
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


#include "Cpl/Text/Frame/Decoder.h"
#include "Cpl/Io/Input.h"


///
namespace Cpl { namespace Text { namespace Frame {



/** This concrete template class provide a basic implemenation of the 
    Frame::Decoder interface that uses Cpl::Io::Input stream as the input
    source.  In addition, this class only accepts ASCCI characters within the
    the frame.

    Template args:
        BUFSIZE     Size of the internal buffer to use when reading raw
                    characters from the Input stream.
 */
template <int BUFSIZE>
class StreamDecoder: public Decoder
{
protected:
    /// My input source
    Cpl::Io::Input* m_srcPtr;

    /// SOF character
    const char      m_sof;

    /// EOF character
    const char      m_eof;

    /// Escape character
    const char      m_esc;

    /// Remember printable ASCII characters ONLY option
    const bool      m_restricted;

    /// Current number of characters remaining in my raw input buffer
    int             m_dataLen;

    /// Pointer to the next unprocessed character in my raw input buffer
    char*           m_dataPtr;

    /// Raw input buffer for reading characters in 'chuncks' from my Input stream (i.e. minimize the calls to read())
    char            m_buffer[BUFSIZE];


public:
    /** Constructor.  If 'restrict' is set to true ONLY printable ASCII 
        characters (0x20-0x7E) are accepted inside a frame.  If false, then 
        all ASCII chracters (0x00-0x7F) are accepted inside a frame.  When
        a illegal character is detected, it causes the Decoder's state machine
        to reset and begin searching/looking-for the next start-of-frame
        character.
     */
    StreamDecoder( Cpl::Io::Input& src, char startOfFrame, char endOfFrame, char escapeChar, bool restrict=true );


    /** Constructor.  Same as the above constructor - but no input src is provided.  
        When using this constructor, the client/applicaiton MUST call the 
        setInputSource() method BEFORE calling scan().
     */
    StreamDecoder( char startOfFrame, char endOfFrame, char escapeChar, bool restrict=true );


public:
    /** This method is used to set the input source prior to calling the scan()
        method.  This method MUST be used at least when creating the decoder
        instance without providing an input source.  This method can be called
        multiple times to change the input source.
     */
    void setInputSource( Cpl::Io::Input* src ) throw();

public:
    /// See Cpl::Text::Frame::Decoder
    bool scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize ) throw();

};


/////////////////////////////////////////////////////////////////////////////
//                          IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////

template <int BUFSIZE>
StreamDecoder<BUFSIZE>::StreamDecoder( Cpl::Io::Input& src, char startOfFrame, char endOfFrame, char escapeChar, bool restrict )
:m_srcPtr(&src)
,m_sof(startOfFrame)
,m_eof(endOfFrame)
,m_esc(escapeChar)
,m_restricted(restrict)
,m_dataLen(0)
,m_dataPtr(0)
    {
    }

template <int BUFSIZE>
StreamDecoder<BUFSIZE>::StreamDecoder( char startOfFrame, char endOfFrame, char escapeChar, bool restrict )
:m_srcPtr(0)
,m_sof(startOfFrame)
,m_eof(endOfFrame)
,m_esc(escapeChar)
,m_restricted(restrict)
,m_dataLen(0)
,m_dataPtr(0)
    {
    }


template <int BUFSIZE>
void StreamDecoder<BUFSIZE>::setInputSource( Cpl::Io::Input* src ) throw()
    {
    m_srcPtr = &src;
    }


template <int BUFSIZE>
bool StreamDecoder<BUFSIZE>::scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize ) throw()
    {
    bool  inFrame  = false;
    bool  escaping = false;
    char* framePtr;

    // Trap: no-valid-input source
    if ( !m_srcPtr )
        {
        return false;
        }

    // Zero out size of the found frame
    frameSize = 0;

    // Scan till a frame is found
    for(;;)
        {
        if ( !m_dataLen )
            {
            if ( !m_srcPtr->read( m_buffer, BUFSIZE, m_dataLen ) )
                {
                // Error reading data -->exit scan
                m_dataLen = 0; // Reset my internal count so I start 'over' on the next call (if there is one)
                return false;
                }

            // Reset my data pointer
            m_dataPtr = m_buffer;
            }

        // Process my input buffer one character at a time
        for( ;m_dataLen; m_dataLen--, m_dataPtr++ )
            {
            // OUTSIDE of a frame
            if ( !inFrame )
                {
                if ( *m_dataPtr == m_sof )
                    {
                    inFrame   = true;
                    escaping  = false;
                    frameSize = 0;
                    framePtr  = frame;
                    }
                }

            // INSIDE a frame
            else 
                {
                // Trap illegal characters
                if ( *m_dataPtr > 0x7F || (m_restricted && (*m_dataPtr > 0x7E || *m_dataPtr < 0x20 )) )
                    {
                    inFrame = false;
                    } 

                // No escape sequence in progress
                else if ( !escaping )
                    {
                    // EOF Characer
                    if ( *m_dataPtr == m_eof )
                        {
                        // EXIT routine with a success return code
                        m_dataPtr++;    // Explicitly consume the EOF character (since we are brute force exiting the loop)
                        m_dataLen--;
                        return true;
                        }

                    // Regular character
                    else if ( *m_dataPtr != m_esc )
                        {
                        // Store incoming character into the Client's buffer
                        if ( frameSize < maxSizeOfFrame )
                            {
                            *framePtr++ = *m_dataPtr;
                            frameSize++;
                            }

                        // Exceeded the Client's buffer space -->internal error -->reset my Frame state
                        else
                            {
                            inFrame = false;
                            }
                        }

                    // Start escape sequence
                    else
                        {
                        escaping = true;
                        }
                    }


                // Escape Sequence
                else
                    {
                    // Store the escaped character into the Client's buffer
                    if ( frameSize < maxSizeOfFrame )
                        {
                        escaping    = false;
                        *framePtr++ = *m_dataPtr;
                        frameSize++;
                        }

                    // Exceeded the Client's buffer space -->internal error -->reset my Frame state
                    else
                        {
                        inFrame = false;
                        }
                    }
                }
            }
        }

    // I should never get here!
    return false;
    }



};      // end namespaces
};
};
#endif  // end header latch
