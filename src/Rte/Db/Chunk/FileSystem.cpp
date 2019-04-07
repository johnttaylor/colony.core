/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 


#include "FileSystem.h"
#include "Cpl/Io/File/Api.h"
#include <new>


///
using namespace Rte::Db::Chunk;


//////////////////////////////
FileSystem::FileSystem( const char* myDbFileName )
:m_fdPtr(0)
,m_fname(myDbFileName)
    {
    }


//////////////////////////////
Cpl::Io::File::InputOutputApi* FileSystem::openDatabase( bool& newfile ) noexcept
    {
    // Make the sure DB file is in the closed state to start with
    closeDatabase();

    // Check if the file exists
    if ( !Cpl::Io::File::Api::exists( m_fname ) )
        {
        newfile = true;
        }

    // File exists 
    else
        {
        newfile = false;
    
        // make the specified db file IS a file and IS writeable
        if ( !Cpl::Io::File::Api::isFile( m_fname ) || !Cpl::Io::File::Api::isWriteable( m_fname ) )
            {
            return 0;
            }
        }

    // Open the DB file (is created if does not already exists)
    m_fdPtr = new(m_memFd.m_byteMem) Cpl::Io::File::InputOutput(m_fname);
    if ( !m_fdPtr->isOpened() )
        {
        closeDatabase();
        return 0;
        }
         
    return m_fdPtr;
    }


void FileSystem::closeDatabase() noexcept
    {
    if ( m_fdPtr )
        {
        m_fdPtr->~InputOutput();
        m_fdPtr = 0;
        }
    }


bool FileSystem::deleteDatabase() noexcept
    {
    // Close the DB just in the case
    closeDatabase();

    // Delete the file
    return Cpl::Io::File::Api::remove( m_fname );
    }


