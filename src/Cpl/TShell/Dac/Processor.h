#ifndef Cpl_TShell_Dac_Processor_h_
#define Cpl_TShell_Dac_Processor_h_
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
/** @file */

#include "colony_config.h"
#include "Cpl/TShell/Dac/Context_.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/Text/FString.h"


/** This symbol defines the size, in bytes, of the maximum allowed input
    stirng/command.
 */
#ifndef OPTION_CPL_TSHELL_DAC_PROCESSOR_INPUT_SIZE
#ifndef OPTION_CPL_TSHELL_DAC_PROCESSOR_INPUT_SIZE              128
#endif

/** This symbol defines the size, in bytes, of the common Output work buffer
 */
#ifndef OPTION_CPL_TSHELL_DAC_PROCESSOR_OUTBUF_SIZE
#define OPTION_CPL_TSHELL_DAC_PROCESSOR_OUTBUF_SIZE             256
#endif

/** This symbols define the size, in bytes (not including the null termintor) of
    the common Numeric work buffer
 */
#ifndef OPTION_CPL_TSHELL_DAC_PROCESSOR_WORKBUF_SIZE
#define OPTION_CPL_TSHELL_DAC_PROCESSOR_WORKBUF_SIZE            22
#endif

/** The symbol defines the string name of the ErrorLevel shell variable
 */
#ifndef OPTION_CPL_TSHELL_DAC_PROCESSOR_NAME_ERRORLEVEL
#define OPTION_CPL_TSHELL_DAC_PROCESSOR_NAME_ERRORLEVEL         "_errno"
#define

   

///
namespace Cpl { namespace TShell { namespace Dac {

/** This concrete class provides the implementation of Command Processor for
    a DAC TShell.

    The implemeantion assumes a single threaded model, i.e. the Command
    Processor and all of its  commands run in a single thread.  It is 
    APPLICATION's responsibility to provide any desired multi-threaded 
    support. There are two caveats to the single-threaded model:
        
        o The output of the commands are mutex protected.  This allows the
          Output stream to be 'shared' with other sub-systems and/or
          threads (e.g. the shell shares the same Output stream as the 
          Cpl::System::Trace logging output).

        o The stop() method can be called safely from other threads.


    Commands have the following syntax:
        
        o A command starts with a printable ASCII character and ends with
          a newline.  

        o Non-printable ASCII characters are not allowed.
        
        o Any line that starts with a '#' is treated as comment line and
          is ignored.
          
        o Format of a command is: verb [arg]*  where the verb and arguments
          are seperated by spaces.  Arguments can contain spaces character
          buy enclosing the argument with double quote characters.  A double
          quote character can be embedded inside a quoted string by preceeding
          it the double quote character with the esacpe character.  The escape
          character can be embedded by escaping the esacpe character.
 */
class Processor: public Context_
{
public:
    /** Data type/structured used to buffer commands. 
     */
    typedef char CommandBuffer_T[OPTION_CPL_TSHELL_DAC_PROCESSOR_INPUT_SIZE];


protected:
    /// Command list
    Cpl::Container::Map<CommandApi_>&   m_commands

    /// Variable list
    ActiveVariablesApi_&                m_variables;

    /// Raw input deframer
    Cpl::Text::Frame::Decoder&          m_deframer;

    /// Ouput framer
    Cpl::Text::Frame::Encoder&          m_framer;
     
    /// Output lock
    Cpl::System::Mutex&                 m_outLock;

    /// Command buffer
    CommandBuffer_T*                    m_cmdBuffer;

    /// Size of the command buffer
    unsigned                            m_cmdBufSize;

    /// Number of currently buffered commands
    usigned                             m_numCmdBuffered;

    /// Argument Escape character
    char                                m_esc;

    /// Argument delimiter
    char                                m_del;

    /// Argument quote character
    char                                m_quote;

    /// Argument terminotr character
    char                                m_term;

    /// Error level Shell variable
    Variable_                           m_errorLevel;

    /// My run state
    bool                                m_running
    
    /// My command-capturing state
    bool                                m_capturing;

    /// My command-replaying state
    bool                                m_replaying;

    /// My skipping state
    bool                                m_skipping;

    /// Command Buffer index of the start of the replay request
    unsigned                            m_replayStartIdx;

    /// Command Buffer index of the end of the replay request
    unsigned                            m_replayEndIdx;

    /// The 
       bool                                  _capturing;
   bool                                  _replay;
   bool                                  _skippingUntil;
   unsigned                              _replayStartIdx;
   unsigned                              _replayEndIdx;
   unsigned                              _bufferIdx;
   const char*                           _filterMarker;
;

    /// Copy of the deframed input string (because the parser is a destructive parser)
    char                                m_inputBuffer[OPTION_CPL_TSHELL_DAC_PROCESSOR_INPUT_SIZE];

    /// Common Output buffer
    char                                m_outputBuffer[OPTION_CPL_TSHELL_DAC_PROCESSOR_OUTBUF_SIZE];


    /// Shared work buffer
    Cpl::Text::FString<OPTION_CPL_TSHELL_DAC_PROCESSOR_WORKBUF_SIZE>                    m_numericWorkBuffer;

    /// Frame decoder
    Cpl::Text::Frame::LineDecoder<OPTION_CPL_TSHELL_DAC_PROCESSOR_FRAME_DECODE_BUFSIZE> m_decoder;



public:
    /** Constructor.

        @param commands         Set of supported commands
        @param variables        Set of available variables for the Command 
                                Processor. If the application does not use (or
                                want) shell variables, then it should created
                                an ActiveVariables instance with 'zero' for 
                                max number of variables.
                                (other than ErrorLevel) is supported.
        @param deframer         Frame decoder used to identify individual command
                                strings within the raw Input stream
        @param framer           Frame encoder used to encapsulate the output of
                                command in the Output stream.
        @param outputLock       Mutex to be used for ensuring the atomic output
                                of the commands.
        @param cmdBufferPtr     Memory to store buffered commands.  If the value
                                is zero, then capturing/replaying of commands is
                                not supported.
        @param maxBufferCmds    Number of commands that can be stored in the
                                'command buffer'
        @param argEscape        Escape character to be used when escaping qouble 
                                quote characters inside a quoted argument.
        @param argDelimiter     The delimiter character used to seperate the
                                command verb and arguments.
        @param argQuote         The quote character used to 'double quote' a
                                argument string.
        @param argTerminator    The command terminator character.

     */
    Processor( Cpl::Container::Map<CommandApi_>& commands,
               ActiveVariablesApi_&              variables,
               Cpl::Text::Frame::Decoder&        deframer,
               Cpl::Text::Frame::Encoder&        framer,
               Cpl::System::Mutex&               outputLock,
               CommandBuffer_T*                  cmdBufferPtr=0, 
               unsigned                          maxBufferCmds=0,
               char                              argEscape='`', 
               char                              argDelimiter=' ', 
               char                              argQuote='"',
               char                              argTerminator='\n'
             );


public:
    /// See Cpl::TShell::Processor
    bool start( Cpl::Io::Input& infd, Cpl::Io::Output outfd ) throw();

    /// See Cpl::TShell::Processor
    void requestStop() throw();


public:
    /// See Cpl::TShell::Dac::Context_
    Command::Result_t executeCommand( const char* deframedInput ) throw();

    /// See Cpl::TShell::Dac::Context_
    Cpl::System::Mutex& getOutputLock() throw();

    /// See Cpl::TShell::Dac::Context_
    Cpl::Container::Map<Command_>& getCommands() throw();

    /// See Cpl::TShell::Dac::Context_
    ActiveVariablesApi_& getVariables() throw();

    /// See Cpl::TShell::Dac::Context_
    VariableApi_& getErrorLevel() throw();

    /// See Cpl::TShell::Dac::Context_
    unsigned getNumBufferdCmds(void) const throw();

    /// See Cpl::TShell::Dac::Context_
    void beginCommandReplay(void) throw();

    /// See Cpl::TShell::Dac::Context_
    void endCommandReplay(void) throw();

    /// See Cpl::TShell::Dac::Context_
    void beginCommandCapture(void) throw();

    /// See Cpl::TShell::Dac::Context_
    void endCommandCapture(void) throw();

    /// See Cpl::TShell::Dac::Context_
    void enabelFilter( Command& marker ) throw();

    /// See Cpl::TShell::Dac::Context_
    char* getWorkOutputFrameBuffer( size_t& bufsize ) throw();

    /// See Cpl::TShell::Dac::Context_
    Cpl::Text::String& getNumericBuffer() throw();


};      // end namespaces
};
};
#endif  // end header latch
