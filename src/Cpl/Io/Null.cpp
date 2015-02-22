
#include "Null.h"

//
using namespace Cpl::Io;



//////////////////////
Null::Null()
:m_opened(true)
    {
    }

Null::~Null()
    {
    }


//////////////////////
bool Null::read( char& c )
    {
    return false;
    }

bool Null::read( Cpl::Text::String& destString )
    {
    return false;
    }

bool Null::read( void* buffer, int numBytes, int& bytesRead )
    {
    bytesRead = 0;
    return false;
    }

bool Null::available()
    {
    return false;
    }


//////////////////////
bool Null::write( char c )
    {
    return m_opened;
    }

bool Null::write( const char* string )
    {
    return m_opened;
    }

bool Null::write( const Cpl::Text::String& string )
    {
    return m_opened;
    }

bool Null::write( Cpl::Text::String& formatBuffer, const char* format, ... )
    {
    va_list ap;
    va_start(ap, format);
    vwrite(formatBuffer,format,ap);
    va_end(ap);
    return m_opened;
    }

bool Null::vwrite( Cpl::Text::String& formatBuffer, const char* format, va_list ap )
    {
    formatBuffer.vformat(format,ap);
    return m_opened;
    }

bool Null::write( const void* buffer, int maxBytes )
    {
    return m_opened;
    }

bool Null::write( const void* buffer, int maxBytes, int& bytesWritten )
    {
    bytesWritten = maxBytes;
    return m_opened;
    }


void Null::flush()
    {
    }

void Null::close()
    {
    m_opened = false;
    }
