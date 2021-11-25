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

    print("refreshing...")
    exit(1)
