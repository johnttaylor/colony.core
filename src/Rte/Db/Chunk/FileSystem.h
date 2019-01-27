#ifndef Rte_Db_Chunk_FileSystem_h_
#define Rte_Db_Chunk_FileSystem_h_
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
/** @file */


#include "Rte/Db/Chunk/Media.h"
#include "Cpl/Memory/Aligned.h"
#include "Cpl/Io/File/InputOutput.h"


///
namespace Rte { namespace Db { namespace Chunk {


/** This concrete class implements the Media interface using the CPL's File
    System interfaces
 */
class FileSystem: public Media
{
protected:
    /// Memory for file descriptor
    Cpl::Memory::AlignedClass<Cpl::Io::File::InputOutput> m_memFd;

    /// Pointer to my instantiated File descriptor
    Cpl::Io::File::InputOutput*                           m_fdPtr;

    /// Data base file name
    const char*                                           m_fname;


public:
    /// Constructor
    FileSystem( const char* myDbFileName );


public:
    /// See Rte::Db::Chunk::Media
    Cpl::Io::File::InputOutputApi* openDatabase( bool& newfile ) throw();
    
    /// See Rte::Db::Chunk::Media
    void closeDatabase() throw();

    /// See Rte::Db::Chunk::Media
    bool deleteDatabase() throw();

};                  


};      // end namespaces
};
};
#endif  // end header latch
