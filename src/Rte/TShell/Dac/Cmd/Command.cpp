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

#include "Command.h"
#include <string.h>


///
using namespace Rte::TShell::Dac::Cmd;


////////////////////////////
Command::Command( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList ) throw()
:Cpl::TShell::Dac::Cmd::Command( commandList, verb )
,m_points(modelPointList)
    {
    commandList.insert( *this );
    }

Command::Command( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw()
:Cpl::TShell::Dac::Cmd::Command( commandList, verb, ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance )
,m_points(modelPointList)
    {
    commandList.insert( *this );
    }



////////////////////////////
Cpl::TShell::Dac::Command::Result_T Command::listPoints( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens ) throw()
    {
    Cpl::Text::String&                    outtext   = context.getOutputBuffer();
    Cpl::TShell::Dac::ActiveVariablesApi& vars      = context.getVariables();
    unsigned                              numParms  = tokens.numParameters();
    bool                                  io        = true;

    // Display Points
    if ( numParms >= 2 )
        {
        bool ls = strcmp(tokens.getParameter(1), "ls") == 0;
        bool ll = strcmp(tokens.getParameter(1), "ll") == 0;
   
        // Was this really a 'list' point request?
        if ( ls || ll )
            {
            // Identify Filter (if any)
            const char* filter = 0;
            if ( numParms > 2 )
                {
                Cpl::Text::String& token = context.getTokenBuffer();
                if ( expandText( tokens.getParameter(2), token, vars ) == Command::eSUCCESS )
                    {
                    filter = token;
                    }
                else
                    {
                    // BAD <etext> -->generate an error
                    return Command::eERROR_INVALID_ARGS;
                    }
                }

            // Iterate across all Points
            Rte::TShell::Dac::Point* pointPtr = m_points.first();
            while( pointPtr && io == true )
                {
                outtext = pointPtr->getName();
                if ( !filter || outtext.contains( filter ) )
                    {
                    outputPointInfo( context, io, outtext, *pointPtr, ls? false: true );
                    }
            
                pointPtr = m_points.next( *pointPtr );
                }

            return io? Command::eSUCCESS: Command::eERROR_IO;
            }
        }


    // If I get here NOT a 'list' command
    return Command::eERROR_FAILED;
    }


void Command::outputPointInfo( Cpl::TShell::Dac::Context_& context, bool& io, Cpl::Text::String& outbuffer, Rte::TShell::Dac::Point& dacPoint, bool includeDetails )
    {
    if ( !includeDetails )
        {
        formatPointName( outbuffer, dacPoint );
        io &= context.writeFrame( outbuffer );
        }
    else
        {
        // Housekeeping
        Rte::Point::Api& point      = dacPoint.getModelPoint().getMyPoint_nonThreadSafe(); // NOTE: I CAN 'use' the non-thread safe call because I am NOT any way reading or updating dynamic values for the Point!
        bool             firstTuple = true;
        unsigned         ntuples    = point.getNumTuples();
        unsigned         i;

        // Only display ONE tuple when the Point is a Container
        if ( point.isContainer() )
            {
            ntuples = 1;
            }

        // Iterate through my Tuples
        formatPointName( outbuffer, dacPoint );
        outbuffer += " { ";
        for(i=0; i<ntuples; i++ )
            {
            // Comma seperate my tuples
            if ( !firstTuple )
                {
                outbuffer += ", ";
                }
            else
                {
                firstTuple = false; 
                }

            // Housekeeping
            Rte::Tuple::Api& tuple      = point.getTuple(i);
            unsigned         nelems     = tuple.getNumElements();
            bool             firstElem  = true;
            unsigned         j;

            // Iterate through my Elements
            outbuffer += "(";
            for(j=0; j<nelems; j++)
                {
                // For a container -->indicate the first element is the "in_container" element
                if ( point.isContainer() && firstElem )
                    {
                    outbuffer.formatAppend( "[%s]", tuple.getElement(j).getTypeAsText() );
                    firstElem = false;
                    continue;
                    }

                // Comma seperate my Elements
                else if ( !firstElem )
                    {
                    outbuffer += ", ";
                    }

                else
                    {
                    firstElem = false;
                    }

                outbuffer += tuple.getElement(j).getTypeAsText();
                }
            outbuffer += ")";

            }
        outbuffer += " }";

        // Output details
        io &= context.writeFrame( outbuffer() );
        }
    }


void Command::formatPointName( Cpl::Text::String& outbuffer, Rte::TShell::Dac::Point& dacPoint )
    {
    outbuffer = dacPoint.getName();

    Rte::Point::Api& point = dacPoint.getModelPoint().getMyPoint_nonThreadSafe(); // NOTE: I CAN 'use' the non-thread safe call because I am NOT any way reading or updating dynamic values for the Point!
    if ( point.isContainer() )
        {
        outbuffer.formatAppend( "[%u]", point.getNumTuples() );
        }
    }
