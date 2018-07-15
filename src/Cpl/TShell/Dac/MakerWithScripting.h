#ifndef Cpl_TShell_Dac_MakerWithScripting_h_
#define Cpl_TShell_Dac_MakerWithScripting_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

#include "Cpl/TShell/Dac/Processor.h"
#include "Cpl/TShell/Dac/ActiveVariables.h"
#include "Cpl/Text/Frame/LineDecoder.h"
#include "Cpl/System/Private_.h"

///
namespace Cpl { namespace TShell { namespace Dac {


/** This concrete Template class is a "Maker" that assembles the objects needed
    for a DAC Shell Processor engine that supports 'Scripting'. 

    Template ARGS:
        NUM_VARS   - The maximum number of allocted/supported variables
        BUF_NCMDS  - Maximum number of commands to buffer, i.e. max number of lines in a loop

 */
template<int NUM_VARS, int BUF_NCMDS>
class MakerWithScripting
{
protected:    
    /// Shell variables
    ActiveVariables<NUM_VARS>       m_variables;

    /// Framer for the output
    Cpl::Text::Frame::StreamEncoder m_framer;

    /// Deframer for the input stream
    Cpl::Text::Frame::LineDecoder<OPTION_CPL_TSHELL_DAC_PROCESSOR_INPUT_SIZE> m_deframer; 

    /// Buffer for loops
    Processor::CommandBuffer_T      m_cmdBuffer[BUF_NCMDS];
    
    /// Command Processor
    Processor                       m_processor;


     
public:
    /** Constructor.  The application is responsible for supplying the set of commands and the mutex to ensure atomic
        output.
     */
    MakerWithScripting( Cpl::Container::Map<Cpl::TShell::Dac::Command>& cmdlist, Cpl::System::Mutex& lock=Cpl::System::Locks_::tracing() );

    /// Special Construct to be used when creating an instance statically!
    MakerWithScripting( const char* ignoreThisParameter_usedToCreateAUniqueConstructor, Cpl::Container::Map<Cpl::TShell::Dac::Command>& cmdlist, Cpl::System::Mutex& lock=Cpl::System::Locks_::tracing() );
         


public:
    /// Returns a reference to the Command processor
    Processor& getCommandProcessor(void) throw()    { return m_processor; }

    /// Cast-operator: Short-hand for getCommandProcessor()
    operator Processor& ()                          { return m_processor; }
};



/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////

template<int NUM_VARS, int BUF_NCMDS>
MakerWithScripting<NUM_VARS,BUF_NCMDS>::MakerWithScripting( Cpl::Container::Map<Cpl::TShell::Dac::Command>& cmdlist, Cpl::System::Mutex& lock )
:m_variables()
,m_framer( 0, '\0', '\n', '\0', false )
,m_deframer( 0, ' ' )
,m_processor( cmdlist, m_variables, m_deframer, m_framer, lock, m_cmdBuffer, BUF_NCMDS ) 
    {
    }

template<int NUM_VARS, int BUF_NCMDS>
MakerWithScripting<NUM_VARS,BUF_NCMDS>::MakerWithScripting( const char* ignoreThisParameter_usedToCreateAUniqueConstructor, Cpl::Container::Map<Cpl::TShell::Dac::Command>& cmdlist, Cpl::System::Mutex& lock )
:m_variables(ignoreThisParameter_usedToCreateAUniqueConstructor)
,m_framer( 0, '\0', '\n', '\0', false )
,m_deframer( 0, ' ' )
,m_processor( cmdlist, m_variables, m_deframer, m_framer, lock, m_cmdBuffer, BUF_NCMDS ) 
    {
    }



};      // end namespaces
};
};
#endif  // end header latch
