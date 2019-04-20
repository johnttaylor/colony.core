/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Dm.h"
#include "Cpl/Dm/ModelPoint.h"
#include <string.h>

///
using namespace Cpl::Dm::TShell::Dac;


///////////////////////////
Dm::Dm( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Dm::ModelDatabaseApi& modelDatabase, const char* cmdNameAndDatabaseNumber ) noexcept
	: Cpl::TShell::Dac::Command( commandList, cmdNameAndDatabaseNumber )
	, m_database( modelDatabase )
{
}

Dm::Dm( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Dm::ModelDatabaseApi& modelDatabase, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance, const char* cmdNameAndDatabaseNumber ) noexcept
	: Cpl::TShell::Dac::Command( commandList, cmdNameAndDatabaseNumber, ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance )
	, m_database( modelDatabase )
{
}


///////////////////////////
Cpl::TShell::Dac::Command::Result_T Dm::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, Cpl::Io::Output& outfd ) noexcept
{
	// Initial check for not-enough-tokens
	if ( tokens.numParameters() < 2 )
	{
		return Command::eERROR_MISSING_ARGS;
	}

	// LIST sub-command
	if ( strcmp( tokens.getParameter( 1 ), "ls" ) == 0 )
	{
		// Too many args?
		if ( tokens.numParameters() > 4 )
		{
			return Command::eERROR_EXTRA_ARGS;
		}

		// Get the optional filter arg
		const char* filter = tokens.numParameters() == 3? tokens.getParameter( 2 ): 0;
		
		// Walk the Model database
		Cpl::Dm::ModelPoint* point = m_database.getFirstByName();
		while( point )
		{
			if ( filter == 0 || strstr( point->getName(), filter ) != 0 )
			{
				if ( !context.writeFrame( point->getName() ) )
				{
					return Command::eERROR_IO;
				}

			}
			point = m_database.getNextByName( *point );
		}

		// If I get here -->the command succeeded
		return Command::eSUCCESS;
	}

	// If I get here the command failed!
	return Command::eERROR_FAILED;
}
