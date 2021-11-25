"""
 
Script that sync's the current workspace with the SCM Repository
===============================================================================
usage: evie [common-opts] sync [options] 
       evie [common-opts] sync [options] <label>

Arguments:
    <label>             SCM Label to use as the source of the Sync.  The default
                        operation is to sync to the latest/newest files
                        
Options:
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
    print("{:<13}{}".format( 'sync', "Sync's the workspace with the SCM Repository" ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)


    # Return 'error' since this is just a stub
    exit(1)
