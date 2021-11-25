"""
 
Script that sync's the current workspace with the SCM Repository
===============================================================================
usage: evie [common-opts] sync [options] 
       evie [common-opts] sync [options] <label>

Arguments:
    <label>             SCM Label to use as the source of the Sync.  The default
                        operation is to sync to the latest/newest files
                        
Options:
    --local             Use local repository/stream/cache only, i.e. do not 
                        require a syncrhronization with a remote server. YMMV
                        depending on the underlying SCM system.
    -h, --help          Display help for this command

    
Notes:
    o Returns zero if all files where succesfully synch'd; else non-zero is 
      is returned.    
    
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( "sync", "Sync's the workspace with the SCM Repository" ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Move to main/latest in my staging area
    cmd = 'git checkout master'
    t   = utils.run_shell( cmd, common_args['-v'] )
    _check_results( t, "ERROR: Failed the 'pre-pull' step (ensure that I am latest on main/master)." )

    # Get the latest and greatest
    if ( not args['--local'] ):
        cmd = 'git pull '
        t   = utils.run_shell( cmd, common_args['-v'] )
        _check_results( t, "ERROR: Failed to sync with Remote Repository - mostly a merge scenario/conflict.\n       Note: Use 'git merge --abort' to revert back to the state before the attempted 'git pull'." )

    # Sync to the label - if one was provided
    if ( args['<label>'] ):
        cmd = 'git checkout {}'.format( args['<label>'] )
        t   = utils.run_shell( cmd, common_args['-v'] )
        _check_results( t, "ERROR: Failed sync to the label: {}",  args['<label>'] )
    
    

def _check_results( t, err_msg, clean_commit=False ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        exit( err_msg )
