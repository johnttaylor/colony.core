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

#include "Catch/catch.hpp"
#include "Cpl/Container/Stack.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// 
using namespace Cpl::Container;
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////

int memoryStack_[5];


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "STACK: Validate member functions", "[stack]" )
{
	Stack<int> stack( sizeof( memoryStack_ ) / sizeof( memoryStack_[0] ), memoryStack_ );

	Shutdown_TS::clearAndUseCounter();


	SECTION( "Operations" )
	{
		int  val;

		REQUIRE( stack.isEmpty() == true );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.peekTop( val ) == false );
		REQUIRE( stack.getNumItems() == 0 );
		REQUIRE( stack.getMaxItems() == 5 );

		REQUIRE( stack.push( 10 ) == true );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 1 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val )  );
		REQUIRE( val == 10 );
		REQUIRE( stack.peekTop( val )  );
		REQUIRE( val == 10 );

		REQUIRE( stack.push( 20 ) == true );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 2 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) );
		REQUIRE( val == 20 );

		REQUIRE( stack.push( 30 ) == true );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 3 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val )  );
		REQUIRE( val == 30 );

		REQUIRE( stack.push( 40 ) == true );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 4 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val )  );
		REQUIRE( val == 40 );

		REQUIRE( stack.push( 50 ) == true );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == true );
		REQUIRE( stack.getNumItems() == 5 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ));
		REQUIRE( val == 50 );

		REQUIRE( stack.push( 60 ) == false );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == true );
		REQUIRE( stack.getNumItems() == 5 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val )  );
		REQUIRE( val == 50 );


		REQUIRE( stack.pop( val ) );
		REQUIRE( val == 50 );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 4 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val )  );
		REQUIRE( val == 40 );

		REQUIRE( stack.pop( val ) );
		REQUIRE( val == 40 );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 3 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val )  );
		REQUIRE( val == 30 );

		REQUIRE( stack.push( 60 ) == true );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 4 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val )  );
		REQUIRE( val == 60 );

		REQUIRE( stack.push( 70 ) == true );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == true );
		REQUIRE( stack.getNumItems() == 5 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) );
		REQUIRE( val == 70 );


		REQUIRE( stack.pop( val ) );
		REQUIRE( val == 70 );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 4 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) );
		REQUIRE( val == 60 );

		REQUIRE( stack.pop( val ) );
		REQUIRE( val == 60 );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 3 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) );
		REQUIRE( val == 30 );

		REQUIRE( stack.pop( val ) );
		REQUIRE( val == 30 );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 2 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) );
		REQUIRE( val == 20 );

		REQUIRE( stack.pop( val ) );
		REQUIRE( val == 20 );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 1 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) );
		REQUIRE( val == 10 );

		REQUIRE( stack.pop( val ) );
		REQUIRE( val == 10 );
		REQUIRE( stack.isEmpty() == true );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 0 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) == false );


		REQUIRE( stack.pop( val ) == false);
		REQUIRE( stack.isEmpty() == true );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.peekTop( val ) == false );

		REQUIRE( stack.push( 10 ) == true );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 1 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) );
		REQUIRE( val == 10 );

		REQUIRE( stack.push( 20 ) == true );
		REQUIRE( stack.isEmpty() == false );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 2 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) );
		REQUIRE( val == 20 );

		stack.clearTheStack();
		REQUIRE( stack.isEmpty() == true );
		REQUIRE( stack.isFull() == false );
		REQUIRE( stack.getNumItems() == 0 );
		REQUIRE( stack.getMaxItems() == 5 );
		REQUIRE( stack.peekTop( val ) == false );
	}


	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
