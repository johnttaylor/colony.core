/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "colony_config.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Io/InputOutput.h"
#include <string.h>


#define SECT_     "_0test"



extern void echo_test( Cpl::Io::InputOutput& fd );



////////////////////////////////////////////////////////////////////////////////
namespace {

class Echo : public Cpl::System::Runnable
{
public:
    ///
    Cpl::Io::InputOutput&           m_fd;
    ///
    Cpl::Text::FString<64>          m_rxMsg;
    ///
    Cpl::Text::FString<128>         m_txMsg;

public:
    Echo( Cpl::Io::InputOutput& fd )
        : m_fd( fd )
    {
    }

public:
    void appRun()
    {
        // Throw any trash bytes on startup
        while ( m_fd.available() )
        {
            m_fd.read( m_rxMsg );
        }

        size_t byteCount = 0;
        for ( ;;)
        {
            
            Bsp_Api_toggle_debug1();
            if ( m_fd.read( m_rxMsg ) )
            {
                Bsp_Api_toggle_debug2();
                byteCount += m_rxMsg.length();
                m_txMsg.format( "RX (%6u): [%s]\n", byteCount, m_rxMsg.getString() );
                m_fd.write( m_txMsg );
            }
        }
    }
};


};  // end namespace


////////////////////////////////////////////////////////////////////////////////


void echo_test( Cpl::Io::InputOutput& fd )
{
    printf( "\nStarting Echo test...\n\n" );
    Echo* echoPtr = new(std::nothrow) Echo( fd );
    Cpl::System::Thread::create( *echoPtr, "ECHO" );

    // Start the scheduler
    Cpl::System::Api::enableScheduling();
}


