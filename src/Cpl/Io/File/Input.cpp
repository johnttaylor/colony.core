/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Input.h"
#include "Common_.h"


//
using namespace Cpl::Io::File;


//////////////////////////
Input::Input( const char* fileName )
:m_stream( Common_::open(fileName) )
    {
    }

Input::Input( Cpl::Io::Descriptor streamfd )
:m_stream( streamfd )
    {
    }


Input::~Input()
    {
    m_stream.close();
    }


bool Input::isOpened()
    {
    return m_stream.isOpened();
    }


//////////////////////////
bool Input::read( char& c )
    {
    m_stream.m_inEos = false;
    return m_stream.read( c );
    }

bool Input::read( Cpl::Text::String& destString )
    {
    m_stream.m_inEos = false;
    return m_stream.read( destString );
    }

bool Input::read( void* buffer, int numBytes, int& bytesRead )
    {
    m_stream.m_inEos = false;
    return m_stream.read( buffer, numBytes, bytesRead );
    }

bool Input::available()
    {
    return m_stream.available();
    }

void Input::close()
    {
    m_stream.close();
    }


//////////////////////////
unsigned long Input::currentPos()
    {
    return Common_::currentPos( m_stream.m_inFd );
    }

bool Input::setRelativePos( long deltaOffset )
    {
    return Common_::setRelativePos( m_stream.m_inFd, deltaOffset );
    }

bool Input::setToEof()
    {
    return Common_::setToEof( m_stream.m_inFd );
    }

bool Input::setAbsolutePos( unsigned long newoffset )
    {
    return Common_::setAbsolutePos( m_stream.m_inFd, newoffset );
    }

bool Input::isEof()
    {
    return m_stream.m_inEos;
    }

unsigned long Input::length()
    {
    return Common_::length( m_stream.m_inFd );
    }



