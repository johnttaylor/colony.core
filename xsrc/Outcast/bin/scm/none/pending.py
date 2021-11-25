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

    print("PENDING: " + args['<pkgname>'])

    # Return 'error' since this is just a stub
    exit(1)
