/*-----------------------------------------------------------------------------
 * This file is part of the Colony.Core Project.  The Colony.Core Project is an
 * open source project with a BSD type of licensing agreement.  See the license
 * agreement (license.txt) in the top/ directory or on the Internet at
 * http://integerfox.com/colony.core/license.txt
 *
 * Copyright (c) 2014-2022  John T. Taylor
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/

#include "Fs.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/Io/File/Api.h"
#include <string.h>

///
using namespace Cpl::Io::File::Littlefs::TShell;


namespace {

class Walker : public Cpl::Io::File::Api::DirectoryWalker
{
public:
    Cpl::TShell::Context_& m_context;
    Cpl::Text::String&     m_outtext;
    unsigned               m_count;
public:
    ///
    Walker( Cpl::TShell::Context_& context, Cpl::Text::String& outtext )
        : m_context( context )
        , m_outtext( outtext )
        , m_count( 0 )
    {
    }

public:
    Cpl::Type::Traverser::Status_T item( const char* currentParent, const char* fsEntryName, Cpl::Io::File::Api::Info& entryInfo )
    {
        m_outtext.format( "%s%s %8lu  %s",
                          entryInfo.m_isFile ? "f" : "-",
                          entryInfo.m_isDir ? "d" : "-",
                          entryInfo.m_size,
                          fsEntryName );
        bool io = m_context.writeFrame( m_outtext );
        m_count++;
        return io ? Cpl::Type::Traverser::eCONTINUE : Cpl::Type::Traverser::eABORT;
    }
};

}  // end anonymous namespace

///////////////////////////
Fs::Fs( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
        unsigned                                   numVolumes,
        const char*                                volumeRootPaths[],
        Cpl::TShell::Security::Permission_T        minPermLevel ) noexcept
    : Cpl::TShell::Cmd::Command( commandList, verb, minPermLevel )
    , m_volPaths( volumeRootPaths )
    , m_numVols( numVolumes )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Fs::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();
    bool                            io       = true;

    // Show volumes sub-command
    if ( numParms == 2 && tokens.getParameter( 1 )[0] == 'v' )
    {
        if ( m_numVols == 1 )
        {
            return context.writeFrame( "vol1 root:  /" ) ? Command::eSUCCESS : Command::eERROR_IO;
        }
        else
        {
            for ( unsigned i = 0; i < m_numVols; i++ )
            {
                outtext.format( "vol%d root:  %s", i + 1, m_volPaths[i] );
                io &= context.writeFrame( outtext );
            }

            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }
    }

    // List directory sub-command
    else if ( numParms == 3 && tokens.getParameter( 1 )[0] == 'l' )
    {
        outtext.format( "Listing: %s", tokens.getParameter( 2 ) );
        io &= context.writeFrame( outtext );

        // Trap the use case of the root directory when there are multiple volumes
        unsigned count = 0;
        if ( m_numVols > 1 && ( strcmp( tokens.getParameter( 2 ), "." ) == 0 || strcmp( tokens.getParameter( 2 ), "/" ) == 0 ) )
        {
            count = m_numVols;
            for ( unsigned i = 0; i < m_numVols; i++ )
            {
                outtext.format( "-d           %s", m_volPaths[i] );
                io &= context.writeFrame( outtext );
            }
        }

        // List the directory
        else
        {
            Walker walker( context, outtext );
            io    &= Cpl::Io::File::Api::walkDirectory( tokens.getParameter( 2 ), walker );
            count  = walker.m_count;
        }

        // Display the totals
        if ( io )
        {
            outtext.format( "Total: %u items", count );
            io &= context.writeFrame( outtext );
        }
        return io ? Command::eSUCCESS : Command::eERROR_FAILED;
    }

    // If I get here the command failed!
    return Command::eERROR_FAILED;
}