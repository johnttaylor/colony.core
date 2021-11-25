"""
 
Updates the Native Package Universe from the Foreign Package Universe
===============================================================================
usage: ceres [common-opts] refresh [options]  
       ceres [common-opts] refresh [options] <pkgtop> 

Arguments:
    <pkgtop>            Name of the Package's Top file to update/refresh from
                        the Foreign Universe.
Options:
    -o, --override      Overrides the top file in Native Universe with the 
                        top file from the Foreign Package Universe.
    -h, --help          Display help for this command

    
Notes:
    o If no <pkgtop> file is specified, then all new/missing packages are pulled
      down to the Native Package Universe
    
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'refresh', 'Updates the Native Package Universe from the Foreign UVerse.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Always update to the latest of everything
    utils.push_dir( common_args['--uverse'] )
    cmd = 'evie.py -v -w {} --user "{}" --passwd "{}" --scm git sync'.format( common_args['-w'], common_args['--user'], common_args['--passwd'] )
    t   = utils.run_shell( cmd, common_args['-v'] )
    utils.pop_dir()
    _check_results( t, "ERROR: Failed to refresh Native Package Universe." )
    

#------------------------------------------------------------------------------
def _check_results( t, err_msg, revert=False ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        exit( err_msg )
