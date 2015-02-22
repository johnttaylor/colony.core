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

#include "Key.h"
#include <string.h>



/// 
using namespace Cpl::Container;


/////////////////////////////////////////////////////////////////////////////
int KeyLiteralString::compareKey( const Key& key ) const
    {
    const char* otherPtr = (const char*) key.getRawKey();
    if ( otherPtr )
        {
        if ( m_stringKeyPtr )
            {
            return strcmp(m_stringKeyPtr, otherPtr );
            }
        }        

    return -1;
    }


const void* KeyLiteralString::getRawKey( unsigned* returnRawKeyLenPtr ) const
    {
    if ( returnRawKeyLenPtr )
        {
        *returnRawKeyLenPtr = m_stringKeyPtr? strlen(m_stringKeyPtr): 0;
        }

    return m_stringKeyPtr;    
    }
