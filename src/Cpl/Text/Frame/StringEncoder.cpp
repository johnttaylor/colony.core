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


#include "StringEncoder.h"


///
using namespace Cpl::Text::Frame;


///////////////////////////////////
StringEncoder::StringEncoder( Cpl::Text::String& dst, char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline )
:Encoder_(startOfFrame, endOfFrame, escapeChar, appendNewline)
,m_dst(dst)
    {
    }



///////////////////////////////////
bool StringEncoder::start( char src ) throw()
    {
    m_dst = src;
    return m_dst.truncated() == false;
    }

bool StringEncoder::start() throw()
    {
    m_dst.clear();
    return m_dst.truncated() == false;
    }

bool StringEncoder::append( char src ) throw()
    {
    m_dst += src;
    return m_dst.truncated() == false;
    }

