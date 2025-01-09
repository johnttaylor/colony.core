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
    ///
    Walker( int depth, bool files = true, bool dirs = true )
        : m_depth( depth ), m_files( files ), m_dirs( dirs ), m_contentCheck( true )
    {
    }

public:
    Cpl::Type::Traverser::Status_T item( const char* currentParent, const char* fsEntryName, Api::Info& entryInfo )
    {
        // File check
        m_workName = fsEntryName;
        if ( entryInfo.m_isFile )
        {
            if ( !m_files )
            {
                m_contentCheck = false;
            }

            else if ( m_depth > 2 )
            {
                if ( m_workName != "d1.txt" && m_workName != "d2.txt" && m_workName != "d3.txt" )
                {
                    m_contentCheck = false;
                }
            }
            else if ( m_depth > 1 )
            {
                if ( m_workName != "d1.txt" && m_workName != "d2.txt" )
                {
                    m_contentCheck = false;
                }
            }
            else if ( m_workName != "d1.txt" )
            {
                m_contentCheck = false;
            }
        }

        // Dir check
        m_workName = fsEntryName;
        if ( entryInfo.m_isDir )
        {
            if ( !m_dirs )
            {
                m_contentCheck = false;
            }

            else if ( m_depth >= 2 )
            {
                if ( m_workName != "d2" && m_workName != "d3" && m_workName != "d22")
                {
                    m_contentCheck = false;
                }
            }
            else if ( m_depth >= 1 )
            {
                if ( m_workName != "d2" && m_workName != "d22" && m_workName != "d3"  ) 
                {
                    m_contentCheck = false;
                }
            }
            else
            {
                m_contentCheck = false;
            }
        }

        printf( "\n%s%s Parent=%s, item=%s.  (depth=%d, content=%d)", entryInfo.m_isFile ? "f" : "-", entryInfo.m_isDir ? "d" : "-", currentParent, fsEntryName, m_depth, m_contentCheck );
        return Cpl::Type::Traverser::eCONTINUE;
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
    if ( numParms == 2 && tokens.getParameter(1)[0] == 'v' )
    {
        if ( m_numVols == 1 )
        {
            return context.writeFrame( "vol1 root:  /" ) ? Command::eSUCCESS : Command::eERROR_IO;
        }
        else
        {
            for ( unsigned i = 0; i < m_numVols; i++ )
            {
                outtext.format( "vol%d root:  %s", i+1, m_volPaths[i] );
                io &= context.writeFrame( outtext );
            }

            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }
    }

    // List directory sub-command
    else if ( numParms == 3 && tokens.getParameter(1)[0] == 'l'  )
    {
        return context.writeFrame( "ls" ) ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // If I get here the command failed!
    return Command::eERROR_FAILED;
}