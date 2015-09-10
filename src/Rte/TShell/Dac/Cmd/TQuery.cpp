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

#include "TQuery.h"
#include "Cpl/Text/atob.h"
#include "Rte/Point/Query/Text.h"


///
using namespace Rte::TShell::Dac::Cmd;


///////////////////////////
TQuery::TQuery( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList ) throw()
:Command( commandList, "tquery", modelPointList )
    {
    }

TQuery::TQuery( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command( commandList, "tquery", modelPointList, ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance )
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T TQuery::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    // Display Points
    Cpl::TShell::Dac::Command::Result_T result = listPoints(context, tokens);
    if ( result != Command::eERROR_FAILED )
        {
        return result;
        }

    // Housekeeping
    Cpl::TShell::Dac::ActiveVariablesApi& vars     = context.getVariables();
    unsigned                              numParms = tokens.numParameters();

    // Error checking
    if ( tokens.numParameters() < 3 )
        {
        return Command::eERROR_MISSING_ARGS;
        }
    if ( tokens.numParameters() > 3 )
        {
        return Command::eERROR_EXTRA_ARGS;
        }

    // Get Point name (can be <etext>)
    Cpl::Text::String& point = context.getTokenBuffer();
    if ( expandText( tokens.getParameter(1), point, vars ) != Command::eSUCCESS )
        {
        return Command::eERROR_BAD_SYNTAX;
        }

    // Get tuple index (can be <etext>)
    Cpl::Text::String& tuple = context.getTokenBuffer2();
    if ( expandText( tokens.getParameter(2), tuple, vars ) != Command::eSUCCESS )
        {
        return Command::eERROR_BAD_SYNTAX;
        }
    unsigned tupleIdx = 0;
    if ( !Cpl::Text::a2ui( tupleIdx, tuple ) )
        {
        return Command::eERROR_BAD_SYNTAX;
        }

    // Look-up Point
    Rte::TShell::Dac::Point* pointPtr = m_points.find(point);
    if ( !pointPtr )
        {
        return Command::eERROR_INVALID_ARGS;
        }

    // Perform Query
    Cpl::Text::String&      outtext = context.getOutputBuffer();
    Rte::Point::Query::Text query(outtext, pointPtr->getModelPoint(), tupleIdx);
    if ( query.issueQuery() )
        {
        return context.writeFrame( outtext )? Command::eSUCCESS: Command::eERROR_IO;
        }

    // If I get here the command failed!
    return Command::eERROR_FAILED;
    }
