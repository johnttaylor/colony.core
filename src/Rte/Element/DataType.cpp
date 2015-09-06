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

#include "DataType.h"

///
using namespace Rte::Element;


//////////////////////////////////
const char * DataType::toString( int dataTypeId )
    {
    switch( dataTypeId )
        {
        // Handle the unknown/invalid case
        default:
            return 0;

        case INTEGER8:  
            return "INTEGER8";

        case INTEGER16: 
            return "INTEGER16";

        case INTEGER32: 
            return "INTEGER32";

        case INTEGER64: 
            return "INTEGER64";

        case UINTEGER8: 
            return "UINTEGER8";

        case UINTEGER16:
            return "UINTEGER16";

        case UINTEGER32:
            return "UINTEGER32";

        case UINTEGER64:
            return "UINTEGER64";

        case SIZE_T:    
            return "SIZE_T";

        case FLOAT:     
            return "FLOAT";

        case DOUBLE:    
            return "DOUBLE";

        case VOIDPTR:   
            return "VOIDPTR";

        case STRING:    
            return "STRING";

        case BOOLEAN:   
            return "BOOLEAN";

        case TOKEN4:    
            return "TOKEN4";

        case MD5:       
            return "MD5";

        case SEQNUMBER: 
            return "SEQNUMBER";

        case TIMET:     
            return "TIMET";

        case BITFLAGS:  
            return "BITFLAGS";

        case CORAL_ADDR:
            return "CORAL_ADDR";
        }

    // I Should never be able to get here -->but make the compiler happy
    return 0;
    }
