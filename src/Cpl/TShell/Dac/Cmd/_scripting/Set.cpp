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

#include "Cpl/TShell/Dac/Cmd/Set.h"


///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;

///////////////////////////
Set::Set( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Command(commandList, "set")
    {
    }

Set::Set( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command(commandList, "set", ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T Set::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars  = context.getVariables();

    // Error checking
    if ( tokens.numParameters() > 3 )
        {
        return Command::eERROR_EXTRA_ARGS;
        }

    // Delete the variable
    if ( tokens.numParameters() == 2 )
        {
        Cpl::Container::KeyLiteralString name( tokens.getParameter(1) );
        VariableApi*                     varPtr = vars.find( name );
        if ( varPtr )
            {
            vars.remove( *varPtr );
            }

        // Always pass the remove/delete case -->i.e. it is okay to delete a non-existant variable
        return Command::eSUCCESS;
        }


    // Update the variable
    else if ( tokens.numParameters() == 3 )
        {
        Cpl::Container::KeyLiteralString name( tokens.getParameter(1) );
        VariableApi* varPtr = vars.get( name );
        if ( varPtr )
            {
            Cpl::Text::String& token = context.getTokenBuffer();
            if ( expandText( tokens.getParameter(2), token, vars ) == Command::eSUCCESS )
                {
                if ( varPtr->setValue( token() ) )
                    {
                    // Return success code if everything worked
                    return Command::eSUCCESS;
                    }
                }
            }
         }

    // Generate list of variables
    else
        {
        Cpl::Text::String& outtext = context.getOutputBuffer();
        bool               io      = true;
        VariableApi*       varPtr  = vars.first();
        while( varPtr && io == true )
            {
            outtext.format( "%s=%s", varPtr->getName(), varPtr->getValue() );
            io    &= context.writeFrame( outtext );
            varPtr = vars.next( *varPtr );
            }

        outtext.format( "Active User variables: %u. Max allowed User variables: %u", vars.getUserCount(), vars.getMaxUserCount() );
        io &= context.writeFrame( outtext );

        return io? Command::eSUCCESS: Command::eERROR_IO;
        }


    // If I get here the command failed!
    return Command::eERROR_FAILED;
    }
