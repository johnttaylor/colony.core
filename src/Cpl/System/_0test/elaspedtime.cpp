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
#include "Cpl/System/Api.h"
#include "Cpl/System/ElaspedTime.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_elaspedtime(void) {}


#define SECT_     "_0test"
/// 
using namespace Cpl::System;



////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "elaspedtime", "[elaspedtime]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Shutdown_TS::clearAndUseCounter();

    unsigned long            secs;
    unsigned long            msecs;
    ElaspedTime::Precision_T precision;
    ElaspedTime::Precision_T precValue;

    // Get current time
    secs      = ElaspedTime::seconds();
    msecs     = ElaspedTime::milliseconds();
    precision = ElaspedTime::precision();

    // Sleep at least 1.5 second
    Api::sleep( 1500 );
    
    // Verify delta time
    REQUIRE( ElaspedTime::expiredMilliseconds(msecs,1500) == true );
    REQUIRE( ElaspedTime::expiredSeconds(secs,1) == true );
    precValue.m_seconds     = 1;
    precValue.m_thousandths = 500;
    REQUIRE( ElaspedTime::expiredPrecision(precision,precValue) == true );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Post verify: sleep = 1.5") );

    // Get current time
    precision = ElaspedTime::precision();
    secs      = ElaspedTime::seconds();
    msecs     = ElaspedTime::milliseconds();

    // Sleep at least 1.0 second
    Api::sleep( 1000 );
    
    // Verify delta time
    REQUIRE( ElaspedTime::expiredMilliseconds(msecs,1000) == true );
    REQUIRE( ElaspedTime::expiredSeconds(secs,1) == true );
    precValue.m_seconds     = 1;
    precValue.m_thousandths = 0;
    REQUIRE( ElaspedTime::expiredPrecision(precision,precValue) == true );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Post verify: sleep = 1.0") );

    // Get current time
    msecs     = ElaspedTime::milliseconds();
    secs      = ElaspedTime::seconds();
    precision = ElaspedTime::precision();

    // Sleep at least 3.7 second
    Api::sleep( 3700 );
    
    // Verify delta time
    REQUIRE( ElaspedTime::expiredMilliseconds(msecs,3700) == true );
    REQUIRE( ElaspedTime::expiredSeconds(secs,3) == true );
    precValue.m_seconds     = 3;
    precValue.m_thousandths = 700;
    REQUIRE( ElaspedTime::expiredPrecision(precision,precValue) == true );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Post verify: sleep = 3.7") );

    CPL_SYSTEM_TRACE_MSG( SECT_, ("msec=%lu, secs=%lu, prec.sec=%lu, prec.msec=%u", msecs, secs, precision.m_seconds, precision.m_thousandths ));

    int i;
    for(i=0; i<50; i++)
        {
        Api::sleep(21);
        msecs     = ElaspedTime::milliseconds();
        secs      = ElaspedTime::seconds();
        precision = ElaspedTime::precision();
    
        unsigned long prec_as_ms = precision.m_seconds * 1000 + precision.m_thousandths;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("msec(%lu) =? prec(%lu)", msecs, prec_as_ms ));
        REQUIRE( msecs <= prec_as_ms );
        REQUIRE( secs <= precision.m_seconds );
        }

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0 );
    }
