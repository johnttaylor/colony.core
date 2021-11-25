"""
 
Script that checks if there are any pending check ins and/or checked out files
===============================================================================
usage: evie [common-opts] pending [options] <pkgname>

Arguments:
      
Options:
    -h, --help          Display help for this command

    
Notes:
    o Returns non-zero if there is at least one pending/checked out files in 
      the package snapshot; else zero is returned.
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'pending', 'Checks if there are any pending check ins and/or checked out files.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
    
    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    path = os.path.join( common_args['-w'], args['<pkgname>'] )
    utils.push_dir( path )
    t = utils.run_shell( "git diff-index --quiet HEAD" )

    # Re-run the command if verbose is enable (more reliable than making assumptions when the --quiet option is not used) 
    if ( common_args['-v'] ):
        if ( t[0] == 0 ):
            print("No pending changes.") 
        else:
            utils.run_shell( "git diff-index HEAD", True )
    
    # restore original cwd
    utils.pop_dir()
    
    # Return an 'error' if there are any pending changes    
    if ( t[0] != 0 ):
        exit( "ERROR: At least one pending change thats needs to be committed." )


    # check for any pending pushes
    cmd = "git log  @{u}.."
    t = utils.run_shell( cmd )
    if ( t[0] != 0 ):
        exit( "ERROR: Unable to run '{}' to check for pending pushes.".format( cmd ) )
    if ( common_args['-v'] ):
        print(t[1])
    if ( t[1].strip() != "" ):
        exit( "ERROR: At least one commit has not been pushed." )
