/*----------------------------------------------------------------------------- 
* This file is part of the Shift-Right Open Repository.  The Repository is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the root directory or on the Internet at           
* http://www.shift-right.com/openrepo/license.htm                               
*                                                                               
* Copyright (c) 2001-2011 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Cpl/Io/File/Api.h"
#include "Cpl/Io/File/Common_.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


///
using namespace Cpl::Io::File;

/////////////////////////////////////////////////////
Cpl::Io::Descriptor Common_::open( const char* fileEntryName, bool readOnly, bool forceCreate, bool forceEmptyFile )
    {
    // Set open flags as requested
    int flags = readOnly? O_RDONLY: O_RDWR;
    int mode  = 0666;
    if ( forceCreate )
        {
        flags |= O_CREAT;
        }
    if ( forceEmptyFile )
        {
        flags |= O_TRUNC;
        }


    // Open the file
    Cpl::Io::Descriptor fd( ::open( Cpl::Io::File::Api::getNative(fileEntryName), flags, mode ) );
    return fd;
    }

/////////////////////////////////////////////////////
unsigned long Common_::currentPos( Cpl::Io::Descriptor fd )
    {
    if ( fd.m_fd == -1 )
        {
        return 0;
        }

    return lseek( fd.m_fd, 0, SEEK_CUR );
    }


bool Common_::setRelativePos( Cpl::Io::Descriptor fd, long deltaOffset )
    {
    if ( fd.m_fd == -1 )
        {
        return false;
        }

    return lseek( fd.m_fd, deltaOffset, SEEK_CUR ) != -1L;
    }


bool Common_::setAbsolutePos( Cpl::Io::Descriptor fd, unsigned long newoffset )
    {
    if ( fd.m_fd == -1 )
        {
        return false;
        }

    return lseek( fd.m_fd, newoffset, SEEK_SET ) != -1L;
    }


bool Common_::setToEof( Cpl::Io::Descriptor fd )
    {
    if ( fd.m_fd == -1 )
        {
        return false;
        }

    return lseek( fd.m_fd, 0, SEEK_END ) != -1L;
    }



unsigned long Common_::length( Cpl::Io::Descriptor fd )
    {
    if ( fd.m_fd == -1 )
        {
        return 0; // Error -->return zero
        }
    
    unsigned long cur = lseek( fd.m_fd, 0, SEEK_CUR );
    unsigned long len = lseek( fd.m_fd, 0, SEEK_END);
    lseek( fd.m_fd, cur, SEEK_SET );
    return len;
    }

