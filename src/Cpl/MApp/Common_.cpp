/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */

#include "Common_.h"

using namespace Cpl::MApp;

Common_::Common_( Cpl::Container::SList<Api>&   mappList,
                  const char*                   mappName,
                  const char*                   description,
                  const char*                   usage )
    : m_name( mappName )
    , m_description( description )
    , m_usage( usage )
    , m_started( false )
{
    mappList.push( *this );
}

const char* Common_::getName() const noexcept
{
    return m_name;
}

const char* Common_::getDescription() const noexcept
{
    return m_description;
}

const char* Common_::getUsage() const noexcept
{
    return m_usage;
}
