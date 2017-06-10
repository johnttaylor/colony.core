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

#include "Cpl/TShell/Dac/Cmd/Print_.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/format.h"
#include "Cpl/System/ElapsedTime.h"


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;


static void initializeOuttext_( Cpl::Text::String& outtext, bool prependTimeStamp );


///////////////////////////
Print_::Print_( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb ) throw()
:Command(commandList, verb)
    {
    }

Print_::Print_( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command(commandList, verb, ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
    {
    }



///////////////////////////
Cpl::TShell::Dac::Command::Result_T Print_::execute( bool prependTimeStamp, Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars    = context.getVariables();
    Cpl::Text::String&  outtext = context.getOutputBuffer();

    // Error checking
    if ( tokens.numParameters() > 2 )
        {
        return Command::eERROR_EXTRA_ARGS;
        }

    if ( tokens.numParameters() == 1 && !prependTimeStamp )
        {
        return Command::eERROR_MISSING_ARGS;
        }


    // Generated expanded text
    initializeOuttext_( outtext, prependTimeStamp );
    Command::Result_T result = expandText( tokens.getParameter(1), outtext, vars );
    if ( result != Command::eSUCCESS )
        {
        return result;
        }


    // Output the final expanded text
    return context.writeFrame( outtext )? Command::eSUCCESS: Command::eERROR_IO;
    }



void initializeOuttext_( Cpl::Text::String& outtext, bool prependTimeStamp )
    {
    // Generate the current timestamp
    if ( !prependTimeStamp )
        {
        outtext.clear();
        }

    else
        {
        Cpl::Text::formatPrecisionTimeStamp( outtext, Cpl::System::ElapsedTime::precision() );
        outtext += "  ";
        }
    }

