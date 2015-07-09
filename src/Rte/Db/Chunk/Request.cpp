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


#include "Request.h"

//////
using namespace Rte::Db::Chunk;


const char* Request::resultToString( Result_T result ) throw()
    {
    switch( result )
        {
        case eSUCCESS:        return "eSUCCESS";       
        case eEOF:            return "eEOF";           
        case eCORRUPT_DATA:   return "eCORRUPT_DATA";  
        case eERR_OPEN:       return "eERR_OPEN";
        case eERR_FILEIO:     return "eERR_FILEIO";    
        case eERR_WRONG_FILE: return "eERR_WRONG_FILE";
        }            
    return "unknown";
    }

