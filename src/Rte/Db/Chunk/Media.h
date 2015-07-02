#ifndef Rte_Db_Chunk_Media_h_
#define Rte_Db_Chunk_Media_h_
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


#include "Cpl/Io/File/InputOutputApi.h"


///
namespace Rte { namespace Db { namespace Chunk {


/** This abstract class defines the interface for opening/closing a database
    file.  This allows the Chunk Server/Layer to be decoupled from the 
    specifics of opening "files". This allows the Chunk Layer to work with
    a non-filesystem based persistent storage.
 */
class Media
{
public:
    /** This method opens the database 'file' that is associated with the
        instance of this interface. The method returns a pointer to an IO Stream 
        upon success; else zero is returned.  The concrete implementation of 
        this interface is responsible for managing the lifecycle/memory of the 
        returned IO stream.

        The 'newfile' flag is set true if database file did not physically exist 
        before the call to opendDatabase(). The Chunk Layer is responsible for
        signing (i.e. writing the db signature string)  new db files.
     */
    virtual Cpl::Io::File::InputOutputApi* openDatabase( bool& newfile ) throw() = 0;


    /** This method closes the associated database file.  If this method is
        called when the db file is not in the open state - no error is
        generated. 
     */
    virtual void closeDatabase() throw() = 0;


public:
    /** This method is used to delete the associated database 'file'. The
        Chunk Layer is responsible for ONLY calling this method when the
        database file is in the CLOSED state. The method returns true if 
        successful; else false is returned.
     */
    virtual bool deleteDatabase() throw() = 0;


public:
    /// Virtual destructor
    virtual ~Media(){}
};                  


};      // end namespaces
};
};
#endif  // end header latch
