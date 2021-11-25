"""
 
Pushes a Package Archive to one or more Package Vaults
===============================================================================
usage: orc [common-opts] pushv [options] <pkgarchive> 

Arguments:
    <pkgarchive>        Name (and path) of the Package Archive to push
                        

Options:
    --vault FILE        Overrides the default setting of which Package Vault(s)
                        to push the package archive to.  The default vault info
                        is set by the Native Package Universe.
    -o,--override       Overwrites the Pacakge Archive if it already exists in
                        Package Vault.
    -h, --help          Display help for this command

    
Notes:
    
    
    
"""
import os
import utils, vault
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'pushv', 'Pushes a Package Archive to one or more Package Vaults.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # throw error if package does not exist
    if ( not os.path.isfile( args['<pkgarchive>']) ):
        exit( "ERROR: Package Archive - {} - does not exist.".format( args['<pkgarchive>'] ))
        
    # set vault file
    if ( not args['--vault'] ):
        args['--vault'] = os.path.join( common_args['--uverse'], "default.vault" )
    else:
        args['--vault'] = os.path.join( common_args['--uverse'], args['--vault'] )
        
    # push the archive file
    vault.push_archive( args['--vault'], args['<pkgarchive>'], args['--override'] )
                                                               