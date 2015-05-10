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




#include "Cpl/System/Private_.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Frame/LineDecoder.h"
#include "Cpl/Text/Frame/StringEncoder.h"
#include "Cpl/TShell/Dac/ActiveVariables.h"
#include "Cpl/TShell/Dac/Processor.h"
#include "Cpl/TShell/Stdio.h"

#include "Cpl/TShell/Dac/Cmd/Set.h"
#include "Cpl/TShell/Dac/Cmd/Help.h"
#include "Cpl/TShell/Dac/Cmd/Bye.h"
#include "Cpl/TShell/Dac/Cmd/Print.h"
#include "Cpl/TShell/Dac/Cmd/TPrint.h"
#include "Cpl/TShell/Dac/Cmd/Try.h"
#include "Cpl/TShell/Dac/Cmd/Inc.h"

/// 
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

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
#define MAX_CMD_BUFFER_         16
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
Cpl::Container::Map<Cpl::TShell::Dac::Command_>  cmdlist_;
Cpl::Text::Frame::LineDecoder<FRAME_SIZE_>       deframer_;
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
Cpl::TShell::Dac::Cmd::Set                       setCmd_( cmdlist_ );
Cpl::TShell::Dac::Cmd::Print                     printCmd_( cmdlist_ );
Cpl::TShell::Dac::Cmd::TPrint                    tprintCmd_( cmdlist_ );
Cpl::TShell::Dac::Cmd::Try                       tryCmd( cmdlist_ );
Cpl::TShell::Dac::Cmd::Inc                       incCmd( cmdlist_ );



void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
    {
    // Start the shell
    shell_.launch( infd, outfd );
       
    // Start the schedular
    Cpl::System::Api::enableScheduling();
    }


