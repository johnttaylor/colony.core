#ifndef Cpl_TShell_Debug_Context_h_
#define Cpl_TShell_Debug_Context_h_
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
#include "Cpl/TShell/Debug/Command_.h"
#include "Cpl/TShell/Debug/ActiveVariables_.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/Io/Output.h"
#include "Cpl/Container/Map.h"


///
namespace Cpl { namespace TShell { namespace Debug {

/** This Private Namespace class defines a "Context" for a Command Processor.  
    The Context provide common infrastructure, information, buffers, etc. that
    facilitates interaction between the Command Processor and individual
    commands.  The application SHOULD NEVER directly access this interface.

    The Command Processor Context supports limited scripted ability with respect
    to commands.  This include (in some form) the ability to:

    - Shell variables that can be set/read in commands
    - Limited looping with IF/ELSE functionality

    The implemeantion assumes a single threaded model, i.e. the Command
    Processor and all of its  commands run in a single thread.  It is 
    APPLICATION's responsibility to provide any desired multi-threaded 
    support. There are two caveats to the single-threaded model:
        
        - The output of the commands is mutex protected.  This allows the
          Output stream to be 'shared' with other sub-systems and/or
          threads (e.g. the shell shares the same Output stream as the 
          Cpl::System::Trace logging output).

        - The stop() method can be called safely from other threads.
      
 */

class Context_: public Cpl::TShell::Processor
{
public:
    /** This method executes attempts to execute the content of the 
        deframed/decoded 'inputString'.  If a valid command is found in the
        'inputString' is excuted.  The method always return true UNLESS there
        is IO stream error - then false is returned.
     */
    virtual Command::Result_t executeCommand( const char* deframedInput ) throw() = 0;
        

public:
    /// This method returns the original/unaltered/unparsed command string
    virtual const char* getOriginalInput() throw() = 0;

    /// This method returns the lock for ensuring atomic output data 
    virtual Cpl::System::Mutex& getOutputLock() throw() = 0;

    /// This method returns the list of implemented commands
    virtual Cpl::Container::Map<Command_>& getCommands() throw() = 0;


public:
    /// This method 
    virtual ActiveVariables_& getVariables() throw() = 0;
    ///
    virtual Variable_ getErrorLevel() throw() = 0;


public:
    /** The method returns the number of commands currently
        captured in Processor's loop buffer
     */
    virtual unsigned getNumBufferdCmds(void) const throw() = 0;


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


    /** The method cause the processor to skip/filter all 
        commands until the 'marker' command is encounter.  When the marker
        command is found, the filtering is turn off and the marker is executed.
     */
    virtual void enabelFilter( Command& marker ) throw() = 0;


public:
    /// Shared/common buffer to be used when generating the encoded/framed output string 
    virtual char* getWorkOutputFrameBuffer( size_t& bufsize ) throw() = 0;

    ///
    virtual Cpl::Text::String& getWorkBuffer() throw() = 0;


};      // end namespaces
};
};
#endif  // end header latch
