/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Cpl/Io/Socket/InputOutput.h"
#include "Cpl/System/FatalError.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>



///
using namespace Cpl::Io::Socket;


/////////////////////
InputOutput::InputOutput()
:m_fd( -1 )
 	{
	} 

InputOutput::InputOutput( Cpl::Io::Descriptor fd )
:m_fd(fd)
 	{
	}
	
InputOutput::~InputOutput( void )
 	{
    close();
	}
	

///////////////////
void InputOutput::activate( Cpl::Io::Descriptor fd )
    {
    // Only activate if already closed 
    if ( m_fd.m_fd < 0 )
        {
        m_fd = fd;
        }        
    else
        {
        Cpl::System::FatalError::logf( "Cpl:Io::Socket::InputOutput::activate().  Attempting to Activate an already opened stream." );
        }
    }


///////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
    {
    // Throw an error if the socket had already been closed
    if ( m_fd.m_fd < 0 )
        {
        return false;
        }

	// Ignore read requests of ZERO bytes
	if ( numBytes == 0 )
		{
		bytesRead = 0;
		return true;
		}
		
    // perform the read
    bytesRead = recv( m_fd.m_fd, (char*)buffer, numBytes, 0 );
    return bytesRead <=0 ? false: true;
    }

bool InputOutput::available()
    {
    // CURRENTLY NOT SUPPORTED -->RETURN TRUE (as per documenation/contract)
    return true;
    }


//////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
    {
    // Throw an error if the socket had already been closed
    if ( m_fd.m_fd < 0 )
        {
        return false;
        }

	// Ignore write requests of ZERO bytes
	if ( maxBytes == 0 )
		{
		bytesWritten = 0;
		return true;
		}
		
    // perform the write
    bytesWritten = send( m_fd.m_fd, (char*)buffer, maxBytes, MSG_NOSIGNAL );
    return bytesWritten <=0 ? false: true;
    }

void InputOutput::flush()
    {
    // Do not know how to implement using only Posix  (jtt 2-14-2015)
    }

void InputOutput::close()
    {
    if ( m_fd.m_fd >= 0 )
        {
        ::shutdown( m_fd.m_fd, 2 ); // 2 == SHUT_RDWR
        ::close( m_fd.m_fd );
        m_fd.m_fd = -1;
        }
    }
