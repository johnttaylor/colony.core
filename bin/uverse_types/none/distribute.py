"""
 
Updates the Foreign Package Universe from the Native Package Universe
===============================================================================
usage: ceres [common-opts] distribute [options] <pkgtop> 

Arguments:
    <pkgtop>            Name of the Package's Top file to update/distribute to
                        the Foreign Universe.
Options:
    -o, --override      Overrides the top file in Foreign Universe with the 
                        file from the Native Package Universe.
    -h, --help          Display help for this command

    
Notes:
    
    
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'distribute', 'Updates the Foreign Package Universe from the Native UVerse.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    print("distribute...")
    exit(1)
