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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Checksum/Crc16CcittFast.h"
#include "Cpl/Checksum/Crc32EthernetFast.h"


/// 
using namespace Cpl::Checksum;

/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_test(void) {}



////////////////////////////////////////////////////////////////////////////////

#define SECT_               "_0test"

#define DATALEN_            9
#define MAX_CRCLEN_         4

#define CRC_CCITT_xFFFF     0x29B1 // CRC value for the text string "12346789"  (see http://www.zorc.breitbandkatze.de/crc.html)

uint8_t dataBuffer_[DATALEN_+MAX_CRCLEN_] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', 0,  };    
static Cpl::Text::FString<128> outbuf_;
          

/////////////////////////////
static void printBuf( uint8_t* buf, int len, const char* msg )
    {
    int i;
    outbuf_.clear();
    for(i=0; i<len; i++)
        {
        outbuf_.formatAppend( "%02X ", buf[i] );
        }
    
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "%s. Buffer (len=%d): %s", msg, len, outbuf_() ) );
    }


bool test16( const char* testName, Api16& cksum, uint8_t* buffer, int len, uint16_t& sum )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "16BIT: Test for: %s", testName ) );
    printBuf( buffer, len, "Outbound buffer" );
    cksum.reset();
    cksum.accumulate(buffer,len);
    sum = cksum.finalize(buffer+len);
    CPL_SYSTEM_TRACE_MSG( SECT_, ("CKSUM=%04X.", sum ) );

    // Validate a 'inbound' CRC'd buffer
    printBuf( buffer, len+sizeof(uint16_t), "Inbound packet/buffer" );
    cksum.reset();
    cksum.accumulate( buffer, len+sizeof(uint16_t) );
    return cksum.isOkay();
    }


/////////////////////////////
TEST_CASE( "Checksums", "[chksum]" )
    {

    Cpl::System::Shutdown_TS::clearAndUseCounter();


    SECTION( "Crc16 - CCITT - Fast" )
        {
        Crc16CcittFast cksum;
        uint16_t       crc;
        REQUIRE( test16("CRC CCITT-FAST", cksum, dataBuffer_, DATALEN_, crc) );
        REQUIRE( crc == CRC_CCITT_xFFFF );
        }

    
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0 );
    }
