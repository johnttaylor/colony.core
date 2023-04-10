/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */

#include "Test.h"

using namespace Loki::Test;

Test::Test( Cpl::Container::Map<TestApi>& testList, const char* testName )
	: m_mapKey( testName )
	, m_started( false )
	, m_running( false )
{
	testList.insert( *this );
}

const char* Test::getName() const noexcept
{
	return m_mapKey.getKeyValue();
}

const Cpl::Container::Key& Test::getKey() const noexcept
{
	return m_mapKey;
}

bool Test::isPaused() const noexcept
{
	return !m_running;
}
