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


#include "StreamEncoder.h"
#include "Cpl/Io/NewLine.h"
#include "Cpl/System/FatalError.h"


///
using namespace Cpl::Text::Frame;



///////////////////////////////////
StreamEncoder::StreamEncoder( Cpl::Io::Output& dst, char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline )
:m_dst(dst)
,m_inFrame(false)
,m_sof(startOfFrame)
,m_eof(endOfFrame)
,m_esc(escapeChar)
,m_appendNewline(appendNewline)
    {
    }



///////////////////////////////////
bool StreamEncoder::startFrame( void )
    {
    if ( m_inFrame )
        {
        Cpl::System::FatalError::logf( "Cpl::Text::Frame::StreamEncoder::startFrame() - Protocol Error." );
        }
    else
        {
        m_inFrame = true;

        if ( m_dst.write(m_sof) )
            {
            return true;
            }
        }

    return false;
    }

bool StreamEncoder::output( char src )
    {
    if ( !m_inFrame )
        {
        Cpl::System::FatalError::logf( "Cpl::Text::Frame::StreamEncoder::output - Protocol Error." );
        }
    else
        {
        if ( src == m_esc || src == m_eof )
            {
            if ( m_dst.write(m_esc) )
                {
                if ( m_dst.write(src) )
                    {
                    return true;
                    }
                }
            }
        else
            {
            if ( m_dst.write(src) )
                {
                return true;
                }
            }
        }

    return false;
    }


bool StreamEncoder::output( const char* src )
    {
    return output( src, strlen(src) );
    }

bool StreamEncoder::output( const char* src, size_t numBytes )
    {
    size_t i;
    for(i=0; i<numBytes; i++)
        {
        if ( !output(*src++) )
            {
            return false;
            }
        }

    return true;
    }

bool StreamEncoder::endFrame( void )
    {
    if ( !m_inFrame )
        {
        Cpl::System::FatalError::logf( "Cpl::Text::Frame::StreamEncoder::endFrame() - Protocol Error." );
        }
    else
        {
        m_inFrame = false;

        if ( m_dst.write(m_eof) )
            {
            if ( !m_appendNewline )
                {
                return true;
                }            

            // Append Newline (makes it easier to read raw output)
            else
                {
                if ( m_dst.write( Cpl::Io::NewLine::standard() ) )
                    {
                    return true;
                    }
                }
            }
        }

    return false;
    }
