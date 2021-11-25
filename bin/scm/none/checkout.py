"""
 
Script to checkout one or more files from the SCM repository
===============================================================================
usage: evie [common-opts] checkout [options] <pkgname> FILES...

Arguments:
    <pkgname>           Name of the package to archive.  The pkgname must be
                        a 'local package' in the current Workspace.
    FILES               List of files of to checkout
      
Options:
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
    print("{:<13}{}".format( 'checkout', 'Checks out one or more files from the SCM repository.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    print("CHECKOUT: {} {}".format( args['<pkgname>'], args['FILES'] ))

    # Return 'error' since this is just a stub
    exit(1)
