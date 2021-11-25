"""
 
Displays branch/version information of a mounted package 
===============================================================================
usage: orc [common-opts] which [options] <pkgname>

Arguments:
    <pkgname>           Name of package to inspect
    
Options:
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
"""
import os
import symlinks
import utils
from docopt.docopt import docopt

from my_globals import OUTCAST_XPKGS_DIRNAME
from my_globals import OUTCAST_XINC_DIRNAME
from my_globals import OUTCAST_LOCAL_PKG_SUFFIX

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'which', 'Displays branch/version information of a mounted package' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Generate path to loaded package
    lpkg  = args['<pkgname>']
    lpath = common_args['-w'] + os.sep + OUTCAST_XPKGS_DIRNAME() + os.sep + lpkg
        
    # Generate a warning if the external package is not loaded
    lpath = common_args['-w'] + os.sep + OUTCAST_XPKGS_DIRNAME() + os.sep + lpkg
    try:
        linksrc = utils.get_pkg_source( lpath, common_args['-p'], common_args['-w'], OUTCAST_LOCAL_PKG_SUFFIX() )
    except OSError as ex:
        exit( "ERROR: Package: '{}' is not mounted or is an invalid name.".format(lpkg) )
            
    print(( linksrc[1:] )) 
        

