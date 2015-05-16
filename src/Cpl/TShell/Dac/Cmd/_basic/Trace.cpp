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

#include "Cpl/TShell/Dac/Cmd/Trace.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/strip.h"
#include <string.h>



///
using namespace Cpl::TShell::Dac::Cmd;
using namespace Cpl::TShell::Dac;


static const char* info2text_( Cpl::System::Trace::InfoLevel_T level );
static Cpl::System::Trace::InfoLevel_T text2info_( const char* level, bool& errorOccurred );



///////////////////////////
Trace::Trace( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw()
:Command(commandList, "trace")
    {
    }

///////////////////////////
Cpl::TShell::Dac::Command::Result_T Trace::execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw()
    {
    ActiveVariablesApi& vars     = context.getVariables();
    Cpl::Text::String&  token    = context.getTokenBuffer();
    Cpl::Text::String&  outtext  = context.getOutputBuffer();
    unsigned            numParms = tokens.numParameters();
    bool                io       = true;

    // Do nothing if trace was not compiled in
    if ( CPL_SYSTEM_TRACE_IS_COMPILED() == false )
        {
        return context.writeFrame( "CPL Tracing was NOT ACTIVATED at Compiled time" )? Command::eSUCCESS: Command::eERROR_IO;
        }


    // Output trace status
    if ( numParms == 1 )
        {
        // Output Enable sections
        io  &= context.writeFrame( " ");
        io  &= context.writeFrame( "TRACE: Currently Enabled Sections:" );
        io  &= context.writeFrame( "----------------------------------" );
        const char* ptr;
        unsigned    count = CPL_SYSTEM_TRACE_GET_SECTIONS( token );
        if ( count )
            {
            ptr  = Cpl::Text::stripSpace( token );
            }
        while( count-- )
            {
            const char* next = Cpl::Text::stripNotSpace( ptr );
            io &= context.writeFrame( ptr, next-ptr );
            ptr = next;
            }

        // Output Enable Thread Filters
        io  &= context.writeFrame( " ");
        io  &= context.writeFrame( "TRACE: Currently Enabled Thread Filters:" );
        io  &= context.writeFrame( "----------------------------------------" );
        count = CPL_SYSTEM_TRACE_GET_THREAD_FILTERS( token );
        if ( count )
            {
            ptr  = Cpl::Text::stripSpace( token );
            }
        while( count-- )
            {
            const char* next = Cpl::Text::stripNotSpace( ptr );
            io &= context.writeFrame( ptr, next-ptr );
            ptr = next;
            }

        // Runtime state
        outtext.format( "TRACE: Runtime state:= %s, Info Level:= %s", CPL_SYSTEM_TRACE_IS_ENABLED()? "ENABLED": "DISABLED", info2text_( CPL_SYSTEM_TRACE_GET_INFO_LEVEL() ) );
        io &= context.writeFrame( " ");
        io &= context.writeFrame( outtext );
        return io? Command::eSUCCESS: Command::eERROR_IO;
        }


    // Global enable/disable
    if ( numParms == 2 && strcmp(tokens.getParameter(1), "on") == 0 )
        {
        CPL_SYSTEM_TRACE_ENABLE();
        return Command::eSUCCESS;
        }
    if ( numParms == 2 && strcmp(tokens.getParameter(1), "off") == 0 )
        {
        CPL_SYSTEM_TRACE_DISABLE();
        return Command::eSUCCESS;
        }


    // Change info level
    if ( numParms == 3 && strcmp(tokens.getParameter(1), "level") == 0 )
        {
        bool errorOccurred = false;
        CPL_SYSTEM_TRACE_SET_INFO_LEVEL( text2info_( tokens.getParameter(2), errorOccurred )  );
        return errorOccurred? Command::eERROR_INVALID_ARGS: Command::eSUCCESS;
        }

    // If I get here -->the argument(s) where bad
    return Command::eERROR_INVALID_ARGS;
    }


////////////////////////////////////////////////////////////////////////////
const char* info2text_( Cpl::System::Trace::InfoLevel_T level )
    {
    switch( level )
        {
        case Cpl::System::Trace::InfoLevel_T::eNONE:    return "NONE";
        case Cpl::System::Trace::InfoLevel_T::eBRIEF:   return "BRIEF";
        case Cpl::System::Trace::InfoLevel_T::eINFO:    return "INFO";
        case Cpl::System::Trace::InfoLevel_T::eVERBOSE: return "VERBOSE";
        case Cpl::System::Trace::InfoLevel_T::eMAX:     return "MAX";
        }
    
    return "??UNKNOWN??";
    }

Cpl::System::Trace::InfoLevel_T text2info_( const char* level, bool& errorOccurred )
    {
    errorOccurred = false;

    if ( strcmp( level, "none" ) == 0 )
        {
        return Cpl::System::Trace::InfoLevel_T::eNONE;
        }
    else if ( strcmp( level, "brief" ) == 0 )
        {
        return Cpl::System::Trace::InfoLevel_T::eBRIEF;
        }
    else if ( strcmp( level, "info" ) == 0 )
        {
        return Cpl::System::Trace::InfoLevel_T::eINFO;
        }
    else if ( strcmp( level, "verbose" ) == 0 )
        {
        return Cpl::System::Trace::InfoLevel_T::eVERBOSE;
        }
    else if ( strcmp( level, "max" ) == 0 )
        {
        return Cpl::System::Trace::InfoLevel_T::eMAX;
        }

    // If I get here -->there was bad token -->so no change in level
    errorOccurred = true;
    return CPL_SYSTEM_TRACE_GET_INFO_LEVEL();
    }

