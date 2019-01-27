#ifndef Rte_Db_Chunk_Null_h_
#define Rte_Db_Chunk_Null_h_
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
#include "Cpl/Io/File/Null.h"


///
namespace Rte { namespace Db { namespace Chunk {


/** This concrete class implements the Media interface using a 'Null File'.
    The typical use case for this class is as a 'stub' for when the platform's 
    actual persistant storage driver/class/file-interface has not been 
    implemented.
 */
class Null: public Media,
            public Cpl::Io::File::Null
{
public:
    /// Constructor
    Null();


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
