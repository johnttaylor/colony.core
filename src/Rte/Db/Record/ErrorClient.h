#ifndef Rte_Db_Record_ErrorClient_h_
#define Rte_Db_Record_ErrorClient_h_
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

#include "Rte/Db/Chunk/Request.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Record { 


/** This class defines the interface for reporting errors encountered (when 
    reading and writing records to the database) to the Domain Application.
    This interface is informational only, i.e. what (if anything) the
    Domain Applicaiton does with information is NOT directly feedback to
    the Record Layer processing.
 */
class ErrorClient
{
public:
    /** This method reports that a 'File' error occured when trying to load
        the data from the database.  A file error can be a error in accessing
        the non-volatle storage media or the file being read is not the 
        specified DB file.
     */
    virtual void databaseFileOpenError( Rte::Db::Chunk::Request::Result_T errorCode ) = 0;
     
    /** This method reports that at least one record in the database is/was
        corrupted.
     */
    virtual void databaseCorruptionError( void ) = 0;

    /** This method reports that a 'File' error occurred when trying to
        write a record to the database.
     */
    virtual void databaseFileWriteError( const char* recordName ) = 0;


public:
    /** This method reports that the database has been (or attempted to be)
        opened.
     */
    virtual void databaseOpened( void ) = 0;

    /** This method reports that the database has been closed, i.e. 'clear'
        any previously reported errors.
     */
    virtual void databaseClosed( void ) = 0;


public:
    /// Virtual destructor to keep the compiler happy
    virtual ~ErrorClient(void){}


};


};      // end namespaces
};
};
#endif  // end header latch
