"""
 
Script that applies a SCM label to the current contents of the package 
===============================================================================
usage: evie [common-opts] label [options] <pkgname> <label>
       evie [common-opts] label [options] --list [<pattern>]

Arguments:
    <pkgname>           Name of the package to archive.  The pkgname must be
                        a 'local package' in the current Workspace.
    <label>             Name of the Label to apply
    --list              List current labels
    
      
Options:
    [<pattern>]         Optional pattern to filter the list of labels
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
    print("{:<13}{}".format( 'label', 'Applies a SCM label to the current contents of the package' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    print("LABEL: {} {}".format( args['<pkgname>'], args['<label>'] ))

    # Return 'error' since this is just a stub
    exit(1)
