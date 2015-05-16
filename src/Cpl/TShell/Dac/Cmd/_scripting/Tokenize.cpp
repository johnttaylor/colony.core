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

#include "Cpl/TShell/Dac/Cmd/Tokenize.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/Text/atob.h"
#include <string.h>



///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;

///////////////////////////
Tokenize::Tokenize( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Command(commandList, "tokenize")
    {
    }

///////////////////////////
Cpl::TShell::Dac::Command::Result_T Tokenize::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars     = context.getVariables();
    Cpl::Text::String&  etext    = context.getTokenBuffer();
    unsigned            numParms = tokens.numParameters();

    // Error checking
    if ( tokens.numParameters() < 5 )
        {
        return Command::eERROR_MISSING_ARGS;
        }

    // Set tokenizer symbols
    const char* symbols   = tokens.getParameter(1);
    int         len       = strlen(symbols);
    char        delimiter = ' ';
    char        quote     = '"';
    char        escape    = '`';
    if ( *symbols != '*' || len > 1 )
        {
        if ( len < 3 )
            {
            return Command::eERROR_INVALID_ARGS;
            }

        delimiter = symbols[0];
        quote     = symbols[1];
        escape    = symbols[2];
        }


    // Expand the text that will be tokenized
    etext.clear();
    Command::Result_T result = expandText( tokens.getParameter(2), etext, *OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_, vars );
    if ( result != Command::eSUCCESS )
        {
        return result;
        }

    // Tokenize operation
    int dummy = 0;
    Cpl::Text::Tokenizer::TextBlock parser( etext.getBuffer( dummy ), delimiter, '\n', quote, escape );
    if ( parser.isValidTokens() == false )
        {
        return Command::eERROR_FAILED;
        }


    // Store the results
    unsigned varParmIdx = 3;
    unsigned tokIdxIdx  = 4;
    numParms -= 3;
    do
        {
        // Look-up variable name
        Cpl::Container::KeyLiteralString name( tokens.getParameter(varParmIdx) );
        VariableApi*                     varPtr = vars.find( name );
        if ( !varPtr )
            {
            return Command::eERROR_INVALID_ARGS;
            }

        // Convert token index to an integer
        unsigned tokIdx = 0;
        if ( !Cpl::Text::a2ui( tokIdx, tokens.getParameter(tokIdxIdx) ) )
            {
            return Command::eERROR_INVALID_ARGS;
            }

        // Update variable. Note: Silently skip invalid token indexes
        if ( tokIdx < parser.numParameters() )
            {
            varPtr->setValue( parser.getParameter(tokIdx) );
            }

        // Go get the next var/idx pair - if there is one
        numParms   -= 2;
        varParmIdx += 2;
        tokIdxIdx  += 2;
        } while ( numParms >= 2 );


    // Error if incorrect number of arguments, i.e. bad var/idx pairs
    if ( numParms != 0 )
        {
        return Command::eERROR_MISSING_ARGS;
        }


    // If I get here, everything worked!
    return Command::eSUCCESS;
    }
