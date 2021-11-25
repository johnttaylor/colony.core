"""
 
Removes a Package Archive from one or more Package Vaults (use with CAUTION!)
===============================================================================
usage: orc [common-opts] rmvault [options] <pkgarchive> 

Arguments:
    <pkgarchive>        Name (and path) of the Package Archive to remove
                        

Options:
    --vault FILE        Overrides the default setting of which Package Vault(s)
                        to push the package archive to.  The default vault info
                        is set by the Native Package Universe.
    -h, --help          Display help for this command

    
Notes:
    
    
    
"""
import os
import utils, vault
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'rmvault', 'Removes a Package Archive from one or more Package Vaults.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # set vault file
    if ( not args['--vault'] ):
        args['--vault'] = os.path.join( common_args['--uverse'], "default.vault" )
        
    # push the archive file
    vault.remove_archive( args['--vault'], args['<pkgarchive>'] )
                                                               