"""
 
Lists packages in the specified Package Vault
===============================================================================
usage: orc [common-opts] lsvault [options] [<filter>]

Arguments:
    <filter>            Optional wildcard/filter.
    
Options:
    --vault FILE        Overrides the default setting of which Package Vault(s)
                        to push the package archive to.  The default vault info
                        is set by the Native Package Universe.
    --long              Display package size, datetime, etc.
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    
    
    
"""
import os
import symlinks
import utils, vault
from docopt.docopt import docopt

from my_globals import OUTCAST_XPKGS_DIRNAME
from my_globals import OUTCAST_XINC_DIRNAME
from my_globals import OUTCAST_LOCAL_PKG_SUFFIX

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'lsvault', 'Lists packages in the specified Package Vault.' )) 
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
    
    # List all of no filter set
    if ( not args['<filter>'] ):
        args['<filter>'] = '*'
        
    # set vault file
    if ( not args['--vault'] ):
        args['--vault'] = os.path.join( common_args['--uverse'], "default.vault" )
        
    # push the archive file
    vault.list_vault( args['--vault'], args['<filter>'], not args['--long'] )
            

