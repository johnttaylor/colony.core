"""
 
Script that reverts/cancels the checkouts for the speficied files
===============================================================================
usage: evie [common-opts] revert [options] <pkgname> FILES...

Arguments:
    <pkgname>           Name of the package to archive.  The pkgname must be
                        a 'local package' in the current Workspace.
    FILES               List of files of revert
      
Options:
    --use-cwd           Uses the current working directory (instead of changing
                        to the root of the package specified by <pkgname>)
    -h, --help          Display help for this command

    
Notes:
    o Returns zero if all files are successfully checked out; else non-zero is 
      is returned.    
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'revert', 'Reverts/cancels the checkouts for the speficied files' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Set the working directory to package/git workspace
    if ( not args['--use-cwd'] ):
        path = os.path.join( common_args['-w'], args['<pkgname>'] )
        utils.push_dir( path )

    # Revert the list of files
    for f in args['FILES']:
        _issue_cmd( common_args, "git reset HEAD " + f )
        _issue_cmd( common_args, "git checkout HEAD " + f )
        


def _issue_cmd( common_args, cmd ):
    t = utils.run_shell( cmd )
    if ( t[0] != 0 ):
        utils.print_warning( "git command failed. Cmd={}, {}.".format( cmd, t[1] ) )
    elif ( common_args['-v'] ):
        if ( t[1] != None and t[1] != 'None None' and t[1].strip() != '' ):
            print(t[1])
