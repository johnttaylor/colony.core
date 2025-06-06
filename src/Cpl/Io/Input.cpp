/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Input.h"


///
using namespace Cpl::Io;


///////////////////
bool Input::read( char& c )
{
    int bytesRead = 0;
    bool result   = true;
    do
    {
        result = read( &c, sizeof( c ), bytesRead );
    } while ( result && bytesRead == 0 );
    return result;
}


bool Input::read( Cpl::Text::String& destString )
{
    int   bytesRead   = 0;
    int   maxlen      = 0;
    char* buffer      = destString.getBuffer( maxlen );
    bool  result      = read( buffer, maxlen, bytesRead );
    buffer[bytesRead] = '\0';
    return result;
}
