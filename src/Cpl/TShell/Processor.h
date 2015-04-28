#ifndef Cpl_TShell_Context_h_
#define Cpl_TShell_Context_h_
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

#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"
#include "Cpl/Container/Map.h"


///
namespace Cpl { namespace TShell {

/** This class defines the interface a TShell Command Processor.  The
    Command Processor is responsible for scanning the input, determing
    what command (if any) to execute; and then executing the command. The
    Command Processor also supports limited scripted ability with respect
    to commands.  This include (in some form) the ability to:

    - Shell variables that can be set/read in commands
    - Limited looping with IF/ELSE functionality

    The TShell assume that Command Processor and all of its  commands run in a
    single thread.  It is APPLICATION's responsibility to provide any desired
    multi-threaded support. Note: The one caveat to this is that the text 
    output of the Command Processor/Commands is mutexed protected so that
    that the output stream can be safely shared with other threads (e.g. with
    the Cpl::System::Trace logging output).
      
 */

class Processor
{
public:
    /** This method is used to start the Command Processor, i.e. it will
        begin to process commands.  This command will not return until
        the Command Processor self terminates or a Input/Output stream
        error was encounter.  The method returns true if the Command Processor
        self terminated; else false is returned.
        
        NOTE: This method is an 'in-thread' intialization, i.e. not thread
        safe.  The application is RESPONSIBLE for managing threading issues.
     */
    bool start( Cpl::Io::Input& infd, Cpl::Io::Output outfd ) throw() = 0;



public:
    /** The Private Namespace method executes attempts to execute the content
        of the deframed/decoded 'inputString'.  If a valid command is found
        in the 'inputString' is excuted.  The method always return true UNLESS
        there is IO stream error - then false is returned.
     */
    virtual Command::Result_t executeCommand_( const char* deframedInput ) throw() = 0;
        

    /** This Private Namespace method will stop the command processor.
     */
    virtual stop_(void) throw() = 0;


public:
    ///
    virtual const char* getOriginalInput_() throw() = 0;
    ///
    virtual Cpl::System::Mutex& getOutputLock_() throw() = 0;
    ///
    virtual Cpl::Container::Map<Command>& getCommands_() throw() = 0;


public:
    ///
    virtual Cpl::Container::Map<Variable>& getVariables_() throw() = 0;
    ///
    virtual Variable& getErrorLevel_() throw() = 0;


public:
    /** The Private Namespace method returns the number of commands currently
        captured in Processor's loop buffer
     */
    virtual unsigned getNumBufferdCmds_(void) const throw() = 0;

    /** The Private Namespace method replays (from the start) the contents of 
        the Processor's loop buffer.
     */
    virtual void beginCommandReplay_(void) throw() = 0;

    /** The Private Namespace method stops the replay of commands
     */
    virtual void endCommandReplay_(void) throw() = 0;

    /** The Private Namespace method begins the capture (into the Processor's 
        loop buffer) of commands.
     */
    virtual void beginCommandCapture_(void) throw() = 0;

    /** The Private Namespace method stops catpure of commands
     */
    virtual void endCommandCapture_(void) throw() = 0;

    /** The Private Namespace method cause the processor to skip/filter all 
        commands until the 'marker' command is encounter.  When the marker
        command is found, the filtering is turn off and the marker is executed.
     */
    virtual void enabelFilter_( Command& marker ) throw() = 0;


public:
    /// Shared/common buffer to be used when generating the encoded/framed output string 
    virtual char* getWorkOutputFrameBuffer_() throw() = 0;

    ///
    virtual Cpl::Text::String& getWorkBuffer_() throw() = 0;


public:
    /// Virtual destructor
    virtual ~Processor(){}


};      // end namespaces
};
#endif  // end header latch
