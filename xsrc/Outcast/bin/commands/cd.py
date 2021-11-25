"""
 
Changes the current working directory relative to a Package
===============================================================================
usage: orc [common-opts] cd [options] 
       orc [common-opts] cd [options]  <pkgname>
       orc [common-opts] cd [options]  <pkgname> <path>

Arguments:
    <pkgname>           Name of package to use as the base/root directory.  If
                        no pkgname is specified then the cwd is set to the
                        workspace root.
    <path>              Path relative to the pkgname's base/root directory. If
                        no path is specified than the cwd is set to the 
                        pkgname's base/root directory.
                                            
Options:
    -v          
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'

Notes:
    o This command is intended to be executed using the Host Orc Wrapper (e.g.
      'orcw').
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  

"""
from docopt.docopt import docopt
import subprocess, os
import utils, deps

#------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'cd', 'Change CWD based on a Pacakge.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )
    
    # Goto the Workspace root
    if ( args['<pkgname>'] == None ):
        cmd  = 'cd ' +  common_args['-w']
        print(".run: " + cmd)

    # Goto the Package root
    elif ( args['<path>'] == None ):
        path = os.path.join( common_args['-w'], args['<pkgname>'] )
        cmd  = 'cd ' +  path
        print(".run: " + cmd)

    # Goto the specified path relative to Package root
    else:
        path = os.path.join( common_args['-w'], args['<pkgname>'], args['<path>'] )
        cmd  = 'cd ' +  path
        print(".run: " + cmd)       
    
#------------------------------------------------------------------------------
