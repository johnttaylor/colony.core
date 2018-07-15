/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Catch/catch.hpp"
#include "Cpl/Log/Loggers.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"


/// 
using namespace Cpl::Log;

/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_compile(void) {}



////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "compile test", "[compile]" )
    {
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Api& defaultLogger = Loggers::application();
    Api& stdoutLogger  = Loggers::console();
    Api& nullLogger    = Loggers::null();
    Api& fastLogger    = Loggers::fast();
    Api& nvmLogger     = Loggers::history();


    defaultLogger.warning( "Warning: Default Logger=%p", &defaultLogger );
    defaultLogger.info(    "Info:    Default Logger=%p", &defaultLogger );
    defaultLogger.verbose( "Verbose: Default Logger=%p", &defaultLogger );

    stdoutLogger.warning( "Warning: Stdout Logger=%p", &stdoutLogger );
    stdoutLogger.info(    "Info:    Stdout Logger=%p", &stdoutLogger );
    stdoutLogger.verbose( "Verbose: Stdout Logger=%p", &stdoutLogger );

    nullLogger.warning( "Warning: Null Logger=%p", &nullLogger );
    nullLogger.info(    "Info:    Null Logger=%p", &nullLogger );
    nullLogger.verbose( "Verbose: Null Logger=%p", &nullLogger );

    fastLogger.warning( "Warning: Fast Logger=%p", &fastLogger );
    fastLogger.info(    "Info:    Fast Logger=%p", &fastLogger );
    fastLogger.verbose( "Verbose: Fast Logger=%p", &fastLogger );

    nvmLogger.warning( "Warning: Nvm Logger=%p", &nvmLogger );
    nvmLogger.info(    "Info:    Nvm Logger=%p", &nvmLogger );
    nvmLogger.verbose( "Verbose: Nvm Logger=%p", &nvmLogger );


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
    }
