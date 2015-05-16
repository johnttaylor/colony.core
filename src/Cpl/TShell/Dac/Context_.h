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
#include "Cpl/TShell/Dac/Command.h"
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
    virtual Command::Result_T executeCommand( const char* deframedInput, Cpl::Io::Output& outfd, unsigned capturing = 0 ) throw() = 0;
                     

public:
    /// This method returns the lock for ensuring atomic output data 
    virtual Cpl::System::Mutex& getOutputLock() throw() = 0;

    /// This method returns the list of implemented commands
    virtual Cpl::Container::Map<Command>& getCommands() throw() = 0;

    /// This method the set of active/in-use Shell variables
    virtual ActiveVariablesApi& getVariables() throw() = 0;

    /** This method returns the Shell variable that contains the 
        numeric value of the returned Result_T code form the last executed
        command.
     */
    virtual VariableApi& getErrorLevel() throw() = 0;

    /** Returns a reference to the Shell variable that contains the
        unframed output of the last command. 
     */
    virtual VariableApi& getLastOutput() throw() = 0;


public:
    /// This method encodes and outputs the specified message/text.  The method returns false if there was Output Stream error
    virtual bool writeFrame( const char* text ) throw() = 0;

    /// Same as writeFrame(), but only outputs (at most) 'N' bytes as the content of the frame
    virtual bool writeFrame( const char* text, size_t maxBytes ) throw() = 0;


public:
    /** The method replays (from the 'level') the contents of the Processor's 
        loop buffer. The method returns false if/when 'level' is out of
        range, i.e. exceeded the allocated memory for storing level/index
        pairs.
     */
    virtual bool beginCommandReplay( unsigned level ) throw() = 0;


    /** The method stops the replay of commands.       
     */
    virtual void endCommandReplay(void) throw() = 0;


    /** The method begins the capture (into the Processor's loop buffer) of 
        commands.  If 'firstCmd' is NOT null,then the string is store as
        the first caputred command.  The method returns false if/when 'level' 
        is out of range, i.e. exceeded the allocated memory for storing 
        level/index pairs.
     */
    virtual bool beginCommandCapture( unsigned level, const char* firstCmd=0 ) throw() = 0;


    /** The method stops catpure of commands. The method returns if/when
        the command capture buffer overflowed.
     */
    virtual bool endCommandCapture(void) throw() = 0;


public:
    /** The method cause the processor to skip/filter all commands until the 
        'marker' command is encounter.  When the marker command is found, the 
        filtering is turn off and the marker is executed.
     */
    virtual void enableFilter( Command& marker ) throw() = 0;


public:
    /** This method returns a working buffer for a command to format its
        output prior to 'writing the frame'.
     */
    virtual Cpl::Text::String& getOutputBuffer() throw() = 0;

    /** A shared/common working buffer. The buffer is guarented to be large 
        enough hold any valid token from an input frame.
     */
    virtual Cpl::Text::String& getTokenBuffer() throw() = 0;

    /** Provides direct access to the _lastout variable's value - should used with CAUTION!
     */
    virtual Cpl::Text::String& getLastOutValue() throw() = 0;


public:
    /// Virtual destructor
    virtual ~Context_(){}
};


};      // end namespaces
};
};
#endif  // end header latch
