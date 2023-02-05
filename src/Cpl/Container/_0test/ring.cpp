/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2023  Michael N. Moran
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Catch/catch.hpp"
#include "Cpl/Container/Ring/Ring.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>

/// 
using namespace Cpl::Container;
/// 
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "RING Header: Validate header", "[ringheader]" )
    {

	static constexpr unsigned	nItems	= 5;

    Ring::Header	header(nItems + 1);

    Shutdown_TS::clearAndUseCounter();


    SECTION( "Operations" )
        {
        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );

		++header._writeIndex;
        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == false );

		++header._readIndex;

        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );

		header._readIndex	= 0;
		header._writeIndex	= nItems-1;

        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == false );

		header._writeIndex	= nItems;

        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == true );

		header._writeIndex	= nItems;

        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == true );

		header._readIndex	= nItems - 1;

        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == false );

		header._readIndex	= nItems;

        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );
		}

    
    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
    }

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "RING Desc: Validate TxDesc and RxDesc", "[ringtxdesc]" )
    {

	static constexpr unsigned	nItems	= 5;
	static constexpr unsigned	arraySize	= nItems + 1;

    Ring::Header	header(nItems + 1);

	Ring::TxDesc	txdesc(header);
	Ring::RxDesc	rxdesc(header);

    Shutdown_TS::clearAndUseCounter();


    SECTION( "Operations" )
        {
        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );

		uint32_t	rxIndex;

		REQUIRE( rxdesc.alloc(rxIndex) == false );

        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );

		uint32_t	txIndex;

		REQUIRE( txdesc.alloc(txIndex) == true );

        REQUIRE( txIndex == 0 );

        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );

		txdesc.commit();

        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == false );

		REQUIRE( rxdesc.alloc(rxIndex) == true );

        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == false );

		rxdesc.commit();

        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );

		for( unsigned i=0; i < nItems; ++i )
		{
			REQUIRE( txdesc.alloc(txIndex) == true );
			REQUIRE( txIndex == ( (i+1) % arraySize) );
		}

		REQUIRE( txdesc.alloc(txIndex) == false );

		REQUIRE( rxdesc.alloc(rxIndex) == false );

        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );

		for( unsigned i=0; i < nItems; ++i )
		{
			txdesc.commit();
		}

        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == true );
        REQUIRE( txdesc.isFull() == true );

		for( unsigned i=0; i < nItems; ++i )
		{
			REQUIRE( rxdesc.alloc(rxIndex) == true );
			REQUIRE( rxIndex == ( (i+1) % arraySize) );
		}

        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == true );
        REQUIRE( txdesc.isFull() == true );

		for( unsigned i=0; i < nItems; ++i )
		{
			rxdesc.commit();
		}

        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );
        REQUIRE( txdesc.isFull() == false );

		for( unsigned i=0; i < nItems; ++i )
		{
			REQUIRE( txdesc.alloc(txIndex) == true );
			REQUIRE( txIndex == ( i % arraySize) );
		}

		REQUIRE( txdesc.alloc(txIndex) == false );

		REQUIRE( rxdesc.alloc(rxIndex) == false );

		for( unsigned i=0; i < nItems; ++i )
		{
			txdesc.commit();
		}

		for( unsigned i=0; i < nItems; ++i )
		{
			REQUIRE( rxdesc.alloc(rxIndex) == true );
			REQUIRE( rxIndex == ( i % arraySize) );
		}

        REQUIRE( header.isEmpty() == false );
        REQUIRE( header.isFull() == true );
        REQUIRE( txdesc.isFull() == true );

		REQUIRE( rxdesc.alloc(rxIndex) == false );

		rxdesc.commitAll();

        REQUIRE( header.isEmpty() == true );
        REQUIRE( header.isFull() == false );
        REQUIRE( txdesc.isFull() == false );
		}
    
    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
    }

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "RING Simple: Validate member functions", "[ring]" )
    {

	static constexpr unsigned	nItems	= 5;

    Ring::Simple< int, nItems + 1 >	buffer;

    Shutdown_TS::clearAndUseCounter();


    SECTION( "Operations" )
        {
        int item = 0;
        
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        item = 10;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        item = 20;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        item = 30;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        item = 40;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        item = 50;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == true );
        item = 60;
        REQUIRE( buffer.add(item) == false );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == true );

        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( item == 10 );

        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( item == 20 );

        item = 60;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );

        item = 70;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == true );

        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( item == 30 );

        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( item == 40 );

        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( item == 50 );

        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( item == 60 );

        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( item == 70 );

        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        
        item = 10;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );

        item = 20;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );

        buffer.clearTheBuffer();
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        }

    
    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
    }
