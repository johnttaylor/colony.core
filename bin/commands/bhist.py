"""
 
Lists the branch history of a Package
===============================================================================
usage: orc [common-opts] bhist [options] <pkgname>
       orc [common-opts] bhist [options] -f BFILE
       orc [common-opts] bhist [options] -f BFILE test <now> <old>
       orc [common-opts] bhist [options] <pkgnbv>
       
Arguments:
    <pkgname>           Lists the history for the package: 'pkgname'.  The 
                        pkgname must be a 'local package' in the current 
                        Workspace.
    -f BFILE            Lists the branch history contain in the file: BFILE
    <pkgnbv>            Lists the branch history for the package (specified by
                        '<pkgnbv>') in the Native Package Universe.  The format 
                        of 'pkgnbv' is: 'name-branch-version', e.g. 
                        'mypkg-main-1.1.4'
    <now>
    <old>

Options:
    --dot               Generate a '.dot' that can be rendered by GraphViz.
    --graph FORMAT      Converts the GraphViz '.dot' file to the specified
                        document FORMAT.  Example 'formats': "pdf", "svg"
                        Note: the '--graph' option does an implicit '--dot'
    -h, --help          Display help for this command


Common Options:
    See 'orc --help'


Notes:
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
      
"""
import os, tarfile
import utils
import shutil
import errno
from docopt.docopt import docopt

from my_globals import OUTCAST_XPKGS_DIRNAME
from my_globals import OUTCAST_XINC_DIRNAME

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'bhist', 'Lists the branch history of a Package.' ))
    
#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Load the specified file
    if ( args['-f'] ):
        try:
            fd = open( args['-f'], 'r' )
            _parse_and_print( fd, f )
        
        except Exception as ex:
            exit( "ERROR: Unable to open branch log file: {} ({})".format(args['-f'],ex) )

    # Process other forms of specifing package to list the history for
    elif ( args['<pkgname>'] ):
    
        # Load local package
        if ( args['<pkgname>'].find('-') == -1 ):
            f = os.path.join(common_args['-w'], args['<pkgname>'], 'top', 'pkg.branching' )
            if ( os.path.isfile(f) ):
                try:
                    fd = open( f, 'r' )
                    _parse_and_print( fd, f )
        
                except Exception as ex:
                    exit( "ERROR: Unable to open branch log file: {} ({})".format(f,ex) )

        # Use package from Native Uverse
        else:
            f = os.path.join( common_args['--uverse'], args['<pkgname>'] + ".top" )
            
            # Open the child top file
            try:
                tar    = tarfile.open( f )
            except Exception as ex:
                exit("ERROR: Trying to locate/read Package Top File: {} ({})".format(f,ex) )
                
            # Get the branch history
            try:
                fh = tar.extractfile( 'top/pkg.branching' )
            except Exception as ex:
                tar.close()
                exit(0)
 
            # Display history
            _parse_and_print( fh, f )
            tar.close()    
            
            
    # Create .DOT file
    if ( args['--dot'] or args['--graph'] ):
        _create_dot_file( args, n )

    # TEST code
    if ( args['test'] ):
        now_bname, now_ver = utils.standardize_dir_sep(args['<now>']).split(os.sep) 
        old_bname, old_ver = utils.standardize_dir_sep(args['<old>']).split(os.sep) 
        if ( now_bname != n[2].name ):
            exit("ERROR: 'now' must match the current branch, aka the last entry in the pkg.branching" )
        print(utils._inspect_parents( now_bname, now_ver, old_bname, old_ver, n[2] ))
        
#------------------------------------------------------------------------------
def _parse_and_print( fd, f ):
    if ( fd != None ):
        n  = utils.parse_branch_log( fd, f )
        fd.close()
        utils.print_branch_history( n )


def _create_dot_file( args, bhist ):
    # create .DOT file
    oname = "pkg.branching.dot"
    utils.create_branch_dot_file( oname, bhist )
                                       
    # generate the visual graph as a XXX file
    utils.render_dot_file_as_pic( args['--graph'], oname )

