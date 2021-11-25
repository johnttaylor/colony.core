"""
 
Script to checkin one or more files to the SCM repository
===============================================================================
usage: evie [common-opts] checkin [options] <pkgname> FILES...

Arguments:
    <pkgname>           Name of the package to archive.  The pkgname must be
                        a 'local package' in the current Workspace.
    FILES               List of files of to checkin

Options:
    -h, --help          Display help for this command

    
Notes:
    o Returns zero if all of the files where successfully checked; else non-zero 
      is returned.    
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'checkin', 'Checks in one or more files to the SCM repository.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    print("CHECKIN: {} {}".format( args['<pkgname>'], args['FILES'] ))

    # Return 'error' since this is just a stub
    exit(1)
