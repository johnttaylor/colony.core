"""
 
Creates a template Package Vault file
===============================================================================
usage: orc [common-opts] mkvault [options] <name>
       orc [common-opts] mkvault --list

Arguments:
    <name>              Name (without a file extension) for the created Vault 
                        file.
    --list              List the types pf support Package Vaults
    
Options:
    --vault TYPE        Type of vault.  [Default: ftp-anon]
    --path DIR          Path of where to place the created file.
    -o,--override       Overwrites the pkg.specification file if it already exists.
    -h, --help          Display help for this command                  


    
Notes:
    
    
    
"""
import os
import utils, vault
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'mkvault', 'Creates a template Package Vault file.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap list option
    if ( args['--list'] ):
        vault.display_supported_types()
        exit(0)
        
    # set the file path and name
    path = os.getcwd()
    if ( args['--path'] ) :
        path = args['--path']
    f = os.path.join( path, args['<name>'] + '.vault' )
    
    # Check if file already exists
    if ( os.path.exists( f ) ):
        if ( os.path.isfile( f ) ):
            if ( not args['--override'] ):
                exit( "ERROR: File - {} - already exists.".format( f ) )
            else:
                os.remove( f )
                utils.print_warning( "Overwriting file: {}.".format( f ) )
        else:
            exit( "ERROR: {} is an existing directory.".format(f) )
            
    
    # Create content
    cfg = vault.create_template_vault_config( args['--vault'] )
        
    # Output to the file
    with open( f, 'wb' ) as cfgfile:
        cfg.write(cfgfile)
    cfgfile.close()

