
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
