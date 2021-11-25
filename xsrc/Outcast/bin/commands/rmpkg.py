"""
 
Marks a published package as Do-Not-Use or deprecated
===============================================================================
usage: orc [common-opts] rmpkg [options] --deprecate  <pkgnbv> <summary>
       orc [common-opts] rmpkg [options] --do-not-use <pkgnbv> <summary>

Arguments:
    --deprecate         Marks a previously published package specified by
                        by '<pkgnbv>' as deprecate.  Warning will be generated
                        when modifing/validating a package that has deprecated
                        dependencies.
    --do-not-use        Marks a previously published package specified by
                        by '<pkgnbv>' as invalid and/or cannot be used.  An 
                        Error will be generated when modifing/validating a 
                        package that has do-not-use dependencies.
    <pkgnbv>            Identifies the package to operate on.  The format of
                        '<pkgnbm>' is: 'name-branch-version', e.g. 
                        'mypkg-main-1.1.4'
    <summary>           Summary/short comments (enclose in quotes)
    
                        
Options:
    -h, --help          Display help for this command

    
Notes:
    
    
    
"""
import os
import utils, vault
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'rmpkg', 'Marks a published package as Do-Not-Use or deprecated.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Housekeeping
    utils.push_dir( common_args['--uverse'] )
    p,b,v   = args['<pkgnbv>'].split('-',2)
    journal = p + ".journal"
    
    # Capture 'remove' time
    now, local = utils.mark_time(common_args['--now'])

    # Ensure the Native Package universe is current
    cmd = 'ceres.py -v -w {} --user "{}" --passwd "{}" refresh'.format(common_args['-w'], common_args['--user'], common_args['--passwd'] )
    t   = utils.run_shell( cmd, common_args['-v'] )
    _check_results( t, "ERROR: Failed Refresh the Native Package Universe." )

    # Lock the journal file
    cmd = 'ceres.py -v -w {} --user "{}" --passwd "{}" lock {}'.format(common_args['-w'], common_args['--user'], common_args['--passwd'], journal )
    t   = utils.run_shell( cmd, common_args['-v'] )
    _check_results( t, "ERROR: Failed to Lock the package's journal file ({}) in the Native/Foreign Universes.".format(journal) )
    
    # deprecate a package
    if ( args['--deprecate'] ):
        err = utils.update_journal_deprecate( journal, common_args['--user'], args['<summary>'], False, v, b )
        if ( err != None ):
            _clean_up( common_args['-w'], p, journal )
            exit( err )
    
    # 'delete' a package
    elif ( args['--do-not-use'] ):
        err = utils.update_journal_do_not_use( journal, common_args['--user'], args['<summary>'], False, v, b )
        if ( err != None ):
            _clean_up( common_args['-w'], p, journal )
            exit( err )
        
    # Commit the changes
    cmd = 'ceres.py -v -w {} --user "{}" --passwd "{}" commit "{}" {}'.format(common_args['-w'], common_args['--user'], common_args['--passwd'], args['<summary>'], journal )
    t   = utils.run_shell( cmd, common_args['-v'] )
    _check_results( t, "ERROR: Failed commit the journal updates to the Native/Foreign Package Universes.", common_args['-w'], journal )

  
#------------------------------------------------------------------------------
def _clean_up( root, journal ):
    cmd = 'ceres.py -v -w {} lock --unlock {}'.format( root, journal )
    utils.run_shell( cmd )
           
def _check_results( t, err_msg, root=None, journal=None ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        if ( root != None ):
            _clean_up( root, journal )
        exit( err_msg )
           