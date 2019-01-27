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

#include "Cpl/TShell/Dac/Cmd/Threads.h"
#include "Cpl/Text/atob.h"
#include "Cpl/System/Shutdown.h"
#include <string.h>         


///
using namespace Cpl::TShell::Dac::Cmd;

///////////////////////////
Threads::Threads( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Command(commandList, "threads")
,m_contextPtr(0)
,m_count(0)
,m_io(true)
    {
    }

Threads::Threads( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command(commandList, "threads", ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
,m_contextPtr(0)
,m_count(0)
,m_io(true)
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T Threads::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    // Error checking
    if ( tokens.numParameters() != 1 )
        {
        return Cpl::TShell::Dac::Command::eERROR_EXTRA_ARGS;
        }

    // House keeping
    Cpl::Text::String& outtext = context.getOutputBuffer();
    m_contextPtr               = &context;
    m_count                    = 0;
    m_io                       = true;

    // Display list header
    m_io &= context.writeFrame( " " );
    outtext.format( "%-16s  %-1s  %-10s  %-10s", "Name", "R", "ID", "Native Hdl" );
    hookHeader1( outtext );
    m_io &= context.writeFrame( outtext );
    outtext.format( "%-16s  %-1s  %-10s  %-10s", "----", "-", "--", "----------" );
    hookHeader2( outtext );
    m_io &= context.writeFrame( outtext );

    // Display the threads
    Cpl::System::Thread::traverse( *this );

    // Finished-up and exit
    outtext.format( "Total number of threads: %u", m_count );
    m_io &= context.writeFrame( " ");
    m_io &= context.writeFrame( outtext );
    return m_io? Command::eSUCCESS: Command::eERROR_IO;
    }


Cpl::Type::Traverser::Status_T Threads::item( Cpl::System::Thread& t )
    {
    Cpl::Text::String& outtext = m_contextPtr->getOutputBuffer();

    m_count++;
    outtext.format( "%-16s  %-1s  %-10p  %-10p", t.getName(), t.isRunning()? "Y" : "n", (void*)(t.getId()), (void*)(t.getNativeHandle()) );
    hookThreadEntry( outtext, t );
    m_io &= m_contextPtr->writeFrame( outtext );
    return Cpl::Type::Traverser::eCONTINUE;
    }


/////////////////////////////////////////////////////////
void Threads::hookHeader1( Cpl::Text::String& text )    
    {
    // Default is: No additional columns
    }

void Threads::hookHeader2( Cpl::Text::String& text )
    {
    // Default is: No additional columns
    }

void Threads::hookThreadEntry( Cpl::Text::String& text, Cpl::System::Thread& currentThread )
    {
    // Default is: No additional columns
    }




