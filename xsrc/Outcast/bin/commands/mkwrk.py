"""
 
Creates a Workspace directory
===============================================================================
usage: orc [common-opts] mkwrk [options] 
       orc [common-opts] mkwrk [options] DIR

Arguments:
    DIR                 Name (and path) of the Workspace directory to create. 

Options:
    --clean             If the 'xpkgs/' and 'xinc/' directories already exist, 
                        they will be deleted.
    --clean-all         If the Workspace directory already exists, it will be
                        deleted. WARNING: USE WITH EXTREME CARE!
    -h, --help          Display help for this command


Common Options:
    See 'orc --help'


Notes:
    o If 'DIR' is omitted, then the current working directory is assumed to be
      the new workspace directory.
    o This command will create the required 'xpkgs/' and 'xinc/' directories as 
       child directories to the Workspace directory.
    o If 'DIR' already exists, then the command will attempt to create the
      'xpgs/' and 'xinc/' sub-directories.
      
"""
import os
import utils
import shutil
import errno
from docopt.docopt import docopt

from my_globals import OUTCAST_XPKGS_DIRNAME
from my_globals import OUTCAST_XINC_DIRNAME

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'mkwrk', 'Creates a Workspace directory.' ))
    
#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # set the workspace dir path
    wrkdir = os.getcwd()
    if ( args['DIR'] ):
        wrkdir = args['DIR']
    wrkdir = utils.standardize_dir_sep( wrkdir )
    xpkgs  = wrkdir + os.sep + OUTCAST_XPKGS_DIRNAME()
    xinc   = wrkdir + os.sep + OUTCAST_XINC_DIRNAME()
    
    # process options
    try:
        if ( args['--clean-all'] ):
            _rmdir( wrkdir )
        
        elif ( args['--clean'] ):
            _rmdir( xpkgs )
            _rmdir( xinc )
        
        # create the workspace directories    
        utils.mkdirs( wrkdir )
        utils.mkdirs( xpkgs )
        utils.mkdirs( xinc )

    except OSError as ex:
        exit( ex )
        

def _rmdir( name ):
    result = False
    try:
        shutil.rmtree( name )
        result = True
    except OSError as ex:
        if ( ex.errno == errno.ENOENT ):
            result = True
        else:
            raise ex
                
    return result  