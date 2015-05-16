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

#include "colony_config.h"
#include "Cpl/System/Api.h"


#define SECT_     "_0test"



#include "Bsp/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Private_.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Frame/LineDecoder.h"
#include "Cpl/Text/Frame/StringEncoder.h"
#include "Cpl/TShell/Dac/ActiveVariables.h"
#include "Cpl/TShell/Dac/Processor.h"
#include "Cpl/TShell/Stdio.h"

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

/// 
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );


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
    const char* getUsage() const throw()   { return "bob on|off [delay]"; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const throw()    { return "  Sets the test trace output to on/off and delay time between msgs"; }
    
    ///
    Apple& m_app;

     
public:
    /// Constructor
    Bob( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Apple& application ) throw()
    :Command(commandList, "bob")
    ,m_app(application)
        {
        }


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
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



////////////////////////////////////////////////////////////////////////////////
#ifndef MAXVARS_
#define MAXVARS_                4
#endif

#define FRAME_SIZE_             OPTION_CPL_TSHELL_DAC_PROCESSOR_INPUT_SIZE

#ifndef SOF_                    
#define SOF_                    '\0'
#endif

#ifndef EOF_
#define EOF_                    '\n'
#endif

#ifndef ESC_
#define ESC_                    '\0'   // Disable Escape sequence -->need for proper 'long line' output
#endif

#ifndef APPEND_NEWLINE_
#define APPEND_NEWLINE_         false
#endif

#ifndef MAX_CMD_BUFFER_
#define MAX_CMD_BUFFER_         64
#endif

#ifndef COMMENT_CHAR_       
#define COMMENT_CHAR_           ';'
#endif

#ifndef ARG_DEL_                    
#define ARG_DEL_                ' '
#endif

#ifndef ARG_QUOTE_
#define ARG_QUOTE_              '"'
#endif

#ifndef ARG_ESC_
#define ARG_ESC_                '`'
#endif

#ifndef ARG_TERM_
#define ARG_TERM_               '\n'
#endif


////////////////////////////////////////////////////////////////////////////////
Cpl::TShell::Dac::ActiveVariables<MAXVARS_>      variables_("invoke_special_static_constructor");
Cpl::Container::Map<Cpl::TShell::Dac::Command>   cmdlist_;
Cpl::Text::Frame::LineDecoder<FRAME_SIZE_>       deframer_(' ');
Cpl::Text::Frame::StreamEncoder                  framer_( 0, SOF_, EOF_, ESC_, APPEND_NEWLINE_ );
Cpl::TShell::Dac::Processor::CommandBuffer_T     cmdBuffer_[MAX_CMD_BUFFER_];
Cpl::TShell::Dac::Processor                      cmdProccessor_( cmdlist_, 
                                                                 variables_, 
                                                                 deframer_, 
                                                                 framer_, 
                                                                 Cpl::System::Locks_::tracing(),
                                                                 cmdBuffer_,
                                                                 MAX_CMD_BUFFER_,
                                                                 COMMENT_CHAR_,
                                                                 ARG_ESC_,
                                                                 ARG_DEL_,
                                                                 ARG_QUOTE_,
                                                                 ARG_TERM_
                                                               );
Cpl::TShell::Stdio                               shell_( cmdProccessor_ );
Cpl::TShell::Dac::Cmd::Help                      helpCmd_( cmdlist_ );
Cpl::TShell::Dac::Cmd::Bye                       byeCmd_( cmdlist_ );
Cpl::TShell::Dac::Cmd::Trace                     traceCmd_( cmdlist_ );
Cpl::TShell::Dac::Cmd::Set                       setCmd_( cmdlist_ );
Cpl::TShell::Dac::Cmd::Print                     printCmd_( cmdlist_ );
Cpl::TShell::Dac::Cmd::TPrint                    tprintCmd_( cmdlist_ );
Cpl::TShell::Dac::Cmd::Try                       tryCmd( cmdlist_ );
Cpl::TShell::Dac::Cmd::Inc                       incCmd( cmdlist_ );
Cpl::TShell::Dac::Cmd::Loop                      loopCmd( cmdlist_ );
Cpl::TShell::Dac::Cmd::Exe                       ExeCmd( cmdlist_ );
Cpl::TShell::Dac::Cmd::Tokenize                  TokenizeCmd( cmdlist_ );


Apple   mockApp;
Bob     bobCmd( cmdlist_, mockApp );


void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
    {
    // Start the shell
    shell_.launch( infd, outfd );
       
    // Create thread for my mock-application to run in
    Cpl::System::Thread::create( mockApp, "APP-BOB" );

    // Start the schedular
    Cpl::System::Api::enableScheduling();
    }


