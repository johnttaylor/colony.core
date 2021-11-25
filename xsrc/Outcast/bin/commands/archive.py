"""
 
Archive file operations
===============================================================================
usage: orc [common-opts] archive [options] <pkgname> <tarname>
       orc [common-opts] archive --ls [--long] <tarname>
       orc [common-opts] archive --list
       
Arguments:
    <pkgname>           Name of the package to archive.  The pkgname must be
                        a 'local package' in the current Workspace.
    <tarname>           Name of the archive file.
    --ls                Lists the contents of the archive file
    --list              Returns a list of Archive formats supported
    --long              Display file details when list an archive contents. 

Options:
    -d PATH             Top level directory to begin the archive from.  PATH is
                        relative to the package's root directory.
    --format FMT        Archive format: tgz, zip, etc. [Default: gztar]
    -o, --override      Will overwrite the archive file it already exists
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
 
 
Notes:
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
   
    
"""
import os
import shutil, tarfile
from docopt.docopt import docopt
import utils, deps

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'archive', 'Archive file operations.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
 
    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Display list of supported formats
    if ( args['--list'] ):
        l = shutil.get_archive_formats()
        for i in l:
            print("{:<13}{}".format( i[0], i[1] ))
        exit(0)
        
    # Display archive contents
    if ( args['--ls'] ):
        try:
            tar = tarfile.open( args['<tarname>'] )
            tar.list(args['--long'])
            tar.close()
        except Exception as ex:
            exit( "ERROR: Unable to list archive file: {} (err={}).".format(args['<tarname>'],ex) )
        exit(0)
        
        
    # Go to the root of the package
    outname = os.path.join( os.getcwd(), args['<tarname>'] )
    ppath   = os.path.join( common_args['-w'], args['<pkgname>'] )
    dir_to_tar = os.path.join(ppath,args['-d']) if args['-d'] else ppath
    
    # Check if source dir exists
    if ( not os.path.isdir(dir_to_tar) ):
        exit( "ERROR: Directory - {} - does not exist".format(dir_to_tar) ) 
               
            
    # Create tar file
    try:
        utils.push_dir( dir_to_tar )
        base = os.path.basename(dir_to_tar)
        root = os.path.dirname(dir_to_tar)

        fn = shutil.make_archive( outname, args['--format'], root, base )

        # Check for existing file
        if ( os.path.isfile(outname) ):
            if ( args['--override'] ):
                utils.print_warning( "Overwriting existing archive file: {}".format( outname ) )
                os.remove( outname )
            else:
                exit( "ERROR: Archive file - {} - already exists".format( outname ) )

        # rename archive file to what the user request (i.e. remove the append 'format' file extension from makearchive())
        os.rename(fn, outname)
            
    except Exception as ex:
        exit( ex )
        
    finally:
        utils.pop_dir()  