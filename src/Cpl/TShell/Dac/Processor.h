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

#include "Cpl/TShell/Debug/Context_.h"


///
namespace Cpl { namespace TShell { namespace Dac {

/** This concrete class provides the implementation of Command Processor for
    a DAC TShell.

    The implemeantion assumes a single threaded model, i.e. the Command
    Processor and all of its  commands run in a single thread.  It is 
    APPLICATION's responsibility to provide any desired multi-threaded 
    support. There are two caveats to the single-threaded model:
        
        - The output of the commands are mutex protected.  This allows the
          Output stream to be 'shared' with other sub-systems and/or
          threads (e.g. the shell shares the same Output stream as the 
          Cpl::System::Trace logging output).

        - The stop() method can be called safely from other threads.
 */

class Processor: public Context_
{
public:
    /// See Cpl::TShell::Processor
    bool start( Cpl::Io::Input& infd, Cpl::Io::Output outfd ) throw();

    /// See Cpl::TShell::Processor
    void stop() throw();


public:
    /// See Cpl::TShell::Dac::Context_
    Command::Result_t executeCommand( const char* deframedInput ) throw();

    /// See Cpl::TShell::Dac::Context_
    const char* getOriginalInput() throw();

    /// See Cpl::TShell::Dac::Context_
    Cpl::System::Mutex& getOutputLock() throw();

    /// See Cpl::TShell::Dac::Context_
    Cpl::Container::Map<Command_>& getCommands() throw();

    /// See Cpl::TShell::Dac::Context_
    ActiveVariables_& getVariables() throw();

    /// See Cpl::TShell::Dac::Context_
    Variable_ getErrorLevel() throw();

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
