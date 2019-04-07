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

#include "colony_config.h"
#include "Cpl/System/Api.h"

#include "Bsp/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Private_.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"

#include "Cpl/TShell/Dac/Cmd/Help.h"
#include "Cpl/TShell/Dac/Cmd/Bye.h"
#include "Cpl/TShell/Dac/Cmd/Trace.h"
#include "Cpl/TShell/Dac/Cmd/Set.h"
#include "Cpl/TShell/Dac/Cmd/Print.h"
#include "Cpl/TShell/Dac/Cmd/TPrint.h"
#include "Cpl/TShell/Dac/Cmd/Try.h"
#include "Cpl/TShell/Dac/Cmd/Inc.h"
#include "Cpl/TShell/Dac/Cmd/Loop.h"
#include "Cpl/TShell/Dac/Cmd/Exe.h"
#include "Cpl/TShell/Dac/Cmd/Tokenize.h"
#include "Cpl/TShell/Dac/Cmd/Strip.h"
#include "Cpl/TShell/Dac/Cmd/ToDec.h"


#define SECT_     "_0test"

#ifndef MAXVARS_
#define MAXVARS_                4
#endif

#ifndef MAX_CMD_BUFFER_
#define MAX_CMD_BUFFER_         64
#endif



////////////////////////////////////////////////////////////////////////////////
namespace {


class Apple: public Cpl::System::Runnable
{
public:
    ///
    Cpl::System::Mutex m_lock;
    ///
    unsigned long      m_delay;
    ///
    bool               m_outputTrace;

public:
    ///
    Apple()
    :m_delay(250)
    ,m_outputTrace(false)
        {
        }

public:
    /// 
    void setOutputState( bool newstate )
        {
        Cpl::System::Mutex::ScopeBlock lock( m_lock );
        m_outputTrace = newstate;
        }

    /// 
    void setDelay( unsigned long newdelay )
        {
        Cpl::System::Mutex::ScopeBlock lock( m_lock );
        m_delay = newdelay;
        }

public:
    ///
    void appRun()
        {
        unsigned counter = 0;
        for(;;)
            {
            Bsp_Api_toggle_debug1();
            counter++;

            m_lock.lock();
            bool          output = m_outputTrace;
            unsigned long delay  = m_delay;
            m_lock.unlock();

            Cpl::System::Api::sleep( delay );
            if ( output )
                {
                CPL_SYSTEM_TRACE_MSG( SECT_, ( "Trace (_0test): loop counter=%u", counter ) );
                CPL_SYSTEM_TRACE_MSG( "bob", ( "Trace (bob): loop counter=%u", counter ) );
                }
            }
        }
};


class Bob: public Cpl::TShell::Dac::Cmd::Command
{
public:
    /// See Cpl::TShell::Dac::Command
    const char* getUsage() const noexcept   { return "bob on|off [delay]"; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const noexcept    { return "  Sets the test trace output to on/off and delay time between msgs"; }
    
    ///
    Apple& m_app;

     
public:
    /// Constructor
    Bob( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Apple& application ) noexcept
    :Command(commandList, "bob")
    ,m_app(application)
        {
        }

    /// Constructor
    Bob( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Apple& application, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) noexcept
    :Command(commandList, "bob", ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
    ,m_app(application)
        {
        }


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) noexcept
        {
        Cpl::Text::String& token = context.getTokenBuffer();

        // Error checking
        if ( tokens.numParameters() > 3 || tokens.numParameters() < 2 )
            {
            return Command::eERROR_INVALID_ARGS;
            }

        // Set output state
        token = tokens.getParameter(1);
        m_app.setOutputState( token == "on"? true: false );

        // Set delay
        if ( tokens.numParameters() > 2 )
            {
            unsigned long newdelay = 250;
            Cpl::Text::a2ul( newdelay, tokens.getParameter(2) );
            m_app.setDelay( newdelay );
            }

        return Command::eSUCCESS;
        }
};


};  // end namespace






