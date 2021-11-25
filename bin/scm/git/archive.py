"""
 
Archives the contents of the specified package while excluding SCM metadata
===============================================================================
usage: evie [common-opts] archive [options] <pkgname> <tarname>
       evie [common-opts] archive --list
       
Arguments:
    <pkgname>           Name of the package to archive.  The pkgname must be
                        a 'local package' in the current Workspace.
    <tarname>           Name of the archive file.
    --list              Returns a list of Archive formats supported

Options:
    -d PATH             Top level directory to begin the archive from.  PATH is
                        relative to the package's root directory.
    --format FMT        Archive format: tgz, zip, etc. [Default: gztar]
    -o, --override      Will overwrite the archive file it already exists
    -h, --help          Display help for this command

Common Options:
    See 'evie --help'
    
Notes:
    o The archive output file is place in the root of the current Workspace.
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
        
"""
import os
import shutil, tarfile
from docopt.docopt import docopt
import utils, deps

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'archive', 'Archive a Package without SCM metadata' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
 
    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Display list of supported formats
    if ( args['--list'] ):
        print("{:<13}{}".format( "bztar", "bzip2'ed tar-file" ))
        print("{:<13}{}".format( "gztar", "gzip'ed tar-file" ))
        print("{:<13}{}".format( "tar",   "uncompressed tar file" ))
        print("{:<13}{}".format( "zip",   "ZIP file" ))
        exit(0)
        
    # Parse the format
    format = 'w:gz'
    if ( args['--format'] == 'bztar' ):
        format = 'w:bz2'
    elif ( args['--format'] == 'tar' ):
        format = 'w'
    elif ( args['--format'] == 'zip' ):
        exit( "ERROR: Zip format not currently supported." )
    elif ( args['--format'] != 'gztar' ):
        exit( "ERROR: Invalid or not-supported Archive format type: {}".format( args['--format'] ) )
        
    # Go to the root of the package
    outname    = os.path.join( common_args['-w'], args['<tarname>'] )
    ppath      = os.path.join( common_args['-w'], args['<pkgname>'] )
    dir_to_tar = os.path.join( ppath, args['-d'] ) if args['-d'] else ppath
    
    # Check if source dir exists
    if ( not os.path.isdir(dir_to_tar) ):
        exit( "ERROR: Directory - {} - does not exist".format(dir_to_tar) ) 
               
            
    # Create tar file
    try:
        # Housekeeping
        global verbose
        verbose = common_args['-v']
        base    = os.path.basename(dir_to_tar)
        root    = os.path.dirname(dir_to_tar)
        utils.push_dir( root )

        # Check for existing file
        if ( os.path.isfile(outname) ):
            if ( args['--override'] ):
                utils.print_warning( "Overwriting existing archive file: {}".format( outname ) )
                os.remove( outname )
            else:
                exit( "ERROR: Archive file - {} - already exists".format( outname ) )
        
        tar = tarfile.open( outname, format )
        tar.add( base, filter=_filter_files ) 
        tar.close()

    except Exception as ex:
        exit( ex )
        
    finally:
        utils.pop_dir()          
        
        
def _filter_files( tarinfo ):
    if ( os.path.basename(tarinfo.name).startswith( ".git" ) ):
        return None
    
    global verbose
    if ( verbose ):
        print(tarinfo.name)
    
    return tarinfo
        