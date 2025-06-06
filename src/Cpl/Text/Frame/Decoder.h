#ifndef Cpl_Text_Frame_Decoder_h_
#define Cpl_Text_Frame_Decoder_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include <stdlib.h>


///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This class defines an interface for 'receiving' (decode) a frame.  What is
	a frame?  A Frame is sequence of characters that has a unique
	start-of-character (SOF) and a unique end-of-frame character (EOF). The
	concept of an escape (ESC) character is also defined to support the content
	of a frame containing SOF/EOF characters.  Below are some examples some
	framed  sequences of characters:
	<pre>

		Given:
			SOF:= '.'
			EOF:= ';'
			ESC:= '~'


		Raw Stream      Decoded Sequence(s)
		-----------     --------------------
		".abcde;"       "abcde"
		".a;.b;.;"      "a", "b", ""
		".a;b.c;"       "a", "c"
		".a~;bcd"       "a;bcd"
		".~~;"          "~"
		".a.bcd;"       "a.bcd"
		".a~.bcd;"      "a.bcd"


		Notes:
			o The quotes (") in the above example are NOT part of the frame
			  and/or character sequences - the quotes are only used to
			  illustrate sets of characters.
			o The SOF character does not need to be escaped within a frame
			  because once a SOF has been found - the SOF character is NOT
			  looked/scanned for until after an EOF character has been
			  detected.  Escaping an SOF character within a frame will
			  behave as expected, i.e. same behavior/semantics as escaping
			  the EOF character.
	</pre>


 */
class Decoder
{
public:
	/** This method reads from an Input source (which is defined/provided by
		the concrete implementation) until a valid frame is found or an error
		occurred.  If a valid frame was found, true will be returned and the
		frame will be stored 'frame'.  The length, in bytes, of the frame found
		is returned via 'frameSize'.  False is returned if a error was
		encountered while reading the Input source.

		CAUTION: The returned frame is NOT a null terminated string - it is
				 ONLY a buffer with 'frameSize' number of characters stored
				 in it.
	 */
	virtual bool scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize ) noexcept = 0;

	/** This method is similar to the above scan() method, except that it does
		NOT block till a 'frame' has found, instead it indicates when a 'frame' 
		has been found by setting the 'isEof' flag to true.

		False is returned if a error was encountered while reading the Input 
		source.

		CAUTION: The returned frame is NOT a null terminated string - it is
				 ONLY a buffer with 'frameSize' number of characters stored
				 in it.
	 */
	virtual bool scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize, bool& isEof ) noexcept = 0;

public:
	/** This method allows 'out-of-band' reading of the input source.  If the
		scanner is in-a-frame the method return false and does nothing.  

		CAUTION: Most client/consumers of the decoder should never use this 
				 method.  This method only has meaning/usefulness when the
				 application KNOWS when the input source is NOT in a frame AND 
				 that there is 'non-framed' data that can be consumed.

		Attempts to read the specified number of bytes from the stream into the
		supplied buffer.  The actual number of bytes read is returned via
		'bytesRead'. Returns true if successful, or false if End-of-Stream
		was encountered or if the input source is in a frame.
	 */
	virtual bool oobRead( void* buffer, int numBytes, int& bytesRead ) noexcept = 0;	

public:
	/// Virtual Destructor
	virtual ~Decoder() {}
};




};      // end namespaces
};
};
#endif  // end header latch
