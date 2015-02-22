#ifndef Cpl_Io_Null_h_
#define Cpl_Io_Null_h_
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

#include "Cpl/Io/InputOutput.h"


///
namespace Cpl { namespace Io {


/** This concrete class implements a NULL InputOutput stream - that
    is all input operations return 'End-of-Stream' (i.e. return false) and all
    output is dropped in the preverbal bit-bucket and goes no where!  Note: The
    write() methods always return true (i.e. no error).
 */
class Null: public InputOutput
{
public:
    /// Constructor
    Null();

    /// Destructor
    ~Null();


public:
    /// See Cpl::Io::Input.
    bool read( char& c );
                                
    /// See Cpl::Io::Input.
    bool read( Cpl::Text::String& destString );

    /// See Cpl::Io::Input.
    bool read( void* buffer, int numBytes, int& bytesRead );

    /// See Cpl::Io::Input.  Note: This method will always return false;
    bool available();


public:
    /// See Cpl::Io::Output
    bool write( char c );

    /// See Cpl::Io::Output
    bool write( const char* string );

    /// See Cpl::Io::Output
    bool write( const Cpl::Text::String& string );

    /// See Cpl::Io::Output.  Note: The output string gets formated - just not outputted
    bool write( Cpl::Text::String& formatBuffer, const char* format, ... );

    /// See Cpl::Io::Output. Note: The output string gets formated - just not outputted
    bool vwrite( Cpl::Text::String& formatBuffer, const char* format, va_list ap );

    /// See Cpl::Io::Output
    bool write( const void* buffer, int maxBytes );

    /// See Cpl::Io::Output.  Note: 'bytesWritten' is set to 'maxBytes'
    bool write( const void* buffer, int maxBytes, int& bytesWritten );

    /// See Cpl::Io::Output.  Note: This method has no meaning for the Null output streamd
    void flush();


public:
    /// See Cpl::Io::Close. Note: Once closed() has been called, all of the write() method will return false;
    void close();


protected:
    /// Track my opened/closed state
    bool m_opened;

};

};      // end namespaces
};
#endif  // end header latch
