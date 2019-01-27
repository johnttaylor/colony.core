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

#include "WriteM.h"
#include "Cpl/Text/atob.h"
#include "Rte/Point/Controller/Text.h"


///
using namespace Rte::TShell::Dac::Cmd;


///////////////////////////
WriteM::WriteM( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList ) throw()
:Command( commandList, "writem", modelPointList )
    {
    }

WriteM::WriteM( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Command( commandList, "writem", modelPointList, ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance )
    {
    }


///////////////////////////
Cpl::TShell::Dac::Command::Result_T WriteM::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
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
    if ( numParms < 3 )
        {
        return Command::eERROR_MISSING_ARGS;
        }

    // Get Point name (can be <etext>)
    Cpl::Text::String& point = context.getTokenBuffer();
    if ( expandText( tokens.getParameter(1), point, vars ) != Command::eSUCCESS )
        {
        return Command::eERROR_BAD_SYNTAX;
        }
    
    // Get start of Point/Tuple data
    const char* pointStart = strchr(rawInputString, '{' );
    const char* tupleStart = strchr(rawInputString, '(' );
    if ( tupleStart < pointStart )
        {
        return Command::eERROR_BAD_SYNTAX;
        }


    // Parse Tuple index (if there is any)
    int tupleIdx = -1;  // Default to: Point write
    if ( !pointStart )
        {
        // Get tuple index (can be <etext>)
        Cpl::Text::String& tuple = context.getTokenBuffer2();
        if ( expandText( tokens.getParameter(2), tuple, vars ) != Command::eSUCCESS )
            {
            return Command::eERROR_BAD_SYNTAX;
            }
        if ( !Cpl::Text::a2i( tupleIdx, tuple ) )
            {
            return Command::eERROR_BAD_SYNTAX;
            }
        }

    // Look-up Point
    Rte::TShell::Dac::Point* pointPtr = m_points.find(point);
    if ( !pointPtr )
        {
        return Command::eERROR_INVALID_ARGS;
        }

    // Get data to write (note: <etext> is supported for values)
    Cpl::Text::String& source = context.getOutputBuffer();
	source.clear();
    if ( expandText( pointStart? pointStart: tupleStart, source, vars ) != Command::eSUCCESS )
        {
        return Command::eERROR_BAD_SYNTAX;
        }

    // Perform Update 
    Rte::Point::Controller::Text controller(source, pointPtr->getModelPoint(), tupleIdx);
    if ( controller.updateModel() )
        {
        return Command::eSUCCESS;
        }

    // If I get here the command failed!
    return Command::eERROR_FAILED;
    }
