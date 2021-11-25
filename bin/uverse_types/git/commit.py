"""
 
Commits changes of one or more Native/Foreign Package Universe metadata files
===============================================================================
usage: ceres [common-opts] commit [options] <summary> FILES... 

Arguments:
    <summary>           Brief comment associated with the commit (enclosed in 
                        qoutes).
    FILES               One or more metadata files in the Native Package
                        Universe that have been PREVIOUSLY locked and have 
                        changes/updates that need to be committed to the Native
                        and Foriegn Universes.
                                            
Options:
    -h, --help          Display help for this command

    
Notes:
    o The commit operation will remove the locks as part of the commit process.
      The lock is removed on succesfull AND unsuccessful completion of the commit
      operation.
    o Unsuccessful operation of the commit operation has the side effect of 
      reverting the changes made to the metadata files.
      
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'commit', 'Commit changes of the Package Universe metadata files' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Housekeeping
    global files, root
    root  = common_args['-w']
    files = ' '.join(args['FILES'])
    
    # GIT does have the concept of global/remote lock -->so the best I can do
    # is checkin and push to the remote/foriegn repository/universe.
    # TODO: Fix me or do not use GIT for Package Universes!
    utils.push_dir( common_args['--uverse'] )
    cmd = 'evie.py -v -w {} --user "{}" --passwd "{}" --scm git checkin --nolabel {} {} "{}" {}'.format(root, common_args['--user'], common_args['--passwd'], 'notused', 'notused', args['<summary>'], files )
    t   = utils.run_shell( cmd, common_args['-v'] )
    _check_results( t, "ERROR: Failed to update Native and Forgien Package Universes with changed file(s): {}.".format(files) )
    utils.pop_dir()
    

#------------------------------------------------------------------------------
def _check_results( t, err_msg ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        exit( err_msg )
        
def _clean_up():
    global files, root
    cmd = 'evie.py -v -w {} --scm git revert --use-cwd notused {}'.format( root, files )
    utils.run_shell( cmd )
                
            
def _check_results( t, err_msg ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        _clean_up()
        exit( err_msg )
