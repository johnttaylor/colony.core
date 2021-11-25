"""
 
Pulls a Package Archive from a Package Vault into the Packages Directory
===============================================================================
usage: orc [common-opts] pullv [options] <pkgnbv>

Arguments:
    <pkgnbv>            Package name, branch, and version to pull from the
                        vault, e.g "my_pkg-main-1.2.3"

Options:
    --vault FILE        Overrides the default setting of which Package Vault(s)
                        to pull the package archive from.  The default vault 
                        info is set by the Native Package Universe.
    -o,--override       Overwrites the Pacakge Archive if it already exists in
                        Packages Directory.
    -h, --help          Display help for this command

    
Notes:
    
    
    
"""
import os, shutil, tarfile
import utils, vault
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'pullv', 'Pulls a Package Archive from a Package Vault.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Build archive name
    archive = args['<pkgnbv>']

    # Check if archive is already in the packages/ directory
    p,b,v = args['<pkgnbv>'].split('-',2)
    tree = os.path.join( common_args['-p'],p, b )
    dst  = os.path.join( tree, v )
    tmp  = os.path.join( tree, "temp" )
    
    if ( os.path.exists(dst) ):
        if ( not args['--override'] ):
            exit( "ERROR: Package ({}) already exists in the Packages/ directory.".format(archive) )
        else:
            utils.print_warning( "Overwriting existing package ({}) in the Packages/ directory.".format(archive) )
                
    
    # set vault file
    if ( not args['--vault'] ):
        args['--vault'] = os.path.join( common_args['--uverse'], "default.vault" )
        
    # pull the archive file
    fd = vault.pull_archive( args['--vault'], archive )
        
    # Remove destination path (if it exists)
    try:
        if ( os.path.isdir(dst) ):
            shutil.rmtree(dst)
        if ( os.path.isdir(tmp) ):
            shutil.rmtree(tmp)
    except Exception as ex:
        exit( "ERROR: Unable to remove previous package in the Packages/ directory (err={})".format(ex) )
    
    # Create destination directory sub-tree
    if ( not os.path.exists(tree) ):
        os.makedirs(tree)
    
    # un-compress the archive to the destination directory
    try:
        utils.print_verbose( "Uncompressing archive..." )
        tar = tarfile.open(fileobj=fd)
        first = tar.next()
        if ( first.name != p ):
            exit( "ERROR: Contents of the archive file - {} - is not an Outcast Package archive.".format( archive ) )
        tar.extractall(tmp)
        tar.close()
    except Exception as ex:
        exit( "ERROR: Unable to uncompress the archive (err={}).".format(ex) )
                                                                   
    finally:
        fd.close()
   
    # clean-up 
    try:
        src = os.path.join( tmp, p )
        os.rename( src, dst )
        shutil.rmtree(tmp)
    except Exception as ex:
        exit( "ERROR: Unable to properly name/clean-up the uncompressed archive directory tree (err={}).".format(ex) )
        