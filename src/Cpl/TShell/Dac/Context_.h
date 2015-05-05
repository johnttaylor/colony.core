#ifndef Cpl_TShell_Dac_ContextApi_x_h_
#define Cpl_TShell_Dac_ContextApi_x_h_
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

#include "Cpl/TShell/Processor.h"
#include "Cpl/TShell/Dac/Command_.h"
#include "Cpl/TShell/Dac/ActiveVariablesApi.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/Io/Output.h"
#include "Cpl/Container/Map.h"


///
namespace Cpl { namespace TShell { namespace Dac {

/** This Private Namespace class defines a "Context" for a DAC shell. The 
    Context provide common infrastructure, information, buffers, etc. that
    facilitates interaction between the Command Processor and individual
    commands.  The application SHOULD NEVER directly access this interface.

    The DAC Context supports limited scripted ability with respect to commands.  
    This include (in some form) the ability to:

    - Shell variables that can be set/read in commands
    - Limited looping with IF/ELSE functionality
 */
class Context_: public Cpl::TShell::Processor
{
public:
    /** This method attempts to execute the content of the deframed/decoded 
        'inputString'.  The method returns the result code of the execute
        command.  If 'inputString' is not a valid command, then the appropriate
        error/result code is returned.
     */
    virtual Command_::Result_T executeCommand( const char* deframedInput, Cpl::Io::Output& outfd ) throw() = 0;
                     

public:
    /// This method returns the lock for ensuring atomic output data 
    virtual Cpl::System::Mutex& getOutputLock() throw() = 0;

    /// This method returns the list of implemented commands
    virtual Cpl::Container::Map<Command_>& getCommands() throw() = 0;

    /// This method the set of active/in-use Shell variables
    virtual ActiveVariablesApi& getVariables() throw() = 0;

    /** This method returns the Shell variable that contains the 
        numeric value of the returned Result_T code form the last executed
        command.
     */
    virtual VariableApi& getErrorLevel() throw() = 0;

    /** Returns a reference to the output frame buffer.  This buffer will
        contained the framed output of the last command.
     */
    virtual Cpl::Text::String& getOutputFrameBuffer() throw() = 0;


public:
    /// Helper method for generating command outputs. Each output 'line' must start with this command
    virtual void startOutput() throw() = 0;

    /// Helper method for generating command outputs. Contents of the output 'line'
    virtual void appendOutput( const char* text ) throw() = 0;

    /// Helper method for generating command outputs. Outputs the content of the cache line to the output stream
    virtual bool commitOutput( Cpl::Io::Output& outfd ) throw() = 0;

    /// Helper method for generating command outputs.  Builds and Commits a single output message
    inline bool outputMessage( Cpl::Io::Output& outfd, const char* text ) throw() { startOutput(); appendOutput(text); return commitOutput(outfd); }


public:
    /** The method replays (from the start) the contents of 
        the Processor's loop buffer.
     */
    virtual void beginCommandReplay(void) throw() = 0;


    /** The method stops the replay of commands
     */
    virtual void endCommandReplay(void) throw() = 0;


    /** The method begins the capture (into the Processor's 
        loop buffer) of commands.
     */
    virtual void beginCommandCapture(void) throw() = 0;


    /** The method stops catpure of commands
     */
    virtual void endCommandCapture(void) throw() = 0;


public:
    /** The method cause the processor to skip/filter all 
        commands until the 'marker' command is encounter.  When the marker
        command is found, the filtering is turn off and the marker is executed.
     */
    virtual void enableFilter( Command_& marker ) throw() = 0;


public:
    /** A shared/common working buffer. The buffer is guarented to be large 
        enough for converting/format binary numbers (int,floats,etc) into
        strings.
     */
    virtual Cpl::Text::String& getNumericBuffer() throw() = 0;


public:
    /// Virtual destructor
    virtual ~Context_(){}
};


};      // end namespaces
};
};
#endif  // end header latch
