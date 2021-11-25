"""
 
Creates a template pkg.specification file
===============================================================================
usage: orc [common-opts] mkspec [options]


Options:
    --path DIR          Path of where to place the created file.
    -o,--override       Overwrites the pkg.specification file if it already exists.
    -h, --help          Display help for this command


Common Options:
    See 'orc --help'


Notes:

      
"""
import os
import utils
import configparser
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'mkspec', 'Creates a template pkg.specification file.' ))
    
#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # set the file path and name
    path = os.getcwd()
    if ( args['--path'] ) :
        path = args['--path']
    f = os.path.join( path, 'pkg.specification' )
    
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
    cfg = configparser.RawConfigParser(allow_no_value=True)
    cfg.optionxform = str
    cfg.add_section( 'info' )
    cfg.set( 'info', 'name',        'my_package' )
    cfg.set( 'info', 'branch',      'main' )
    cfg.set( 'info', 'version',     '0.0.0' )
    cfg.set( 'info', 'pubtime',     '0' )
    cfg.set( 'info', 'pubtimelocal','0' )
    cfg.set( 'info', 'desc',        'Blah...' )
    cfg.set( 'info', 'owner',       'Bob Smith' )
    cfg.set( 'info', 'email',       'BobSmith@notmail.com' )
    cfg.set( 'info', 'url',         '<optional>' )

    cfg.add_section( 'immediate_deps' )
    cfg.add_section( 'weak_deps' )
    cfg.add_section( 'transitive_deps' )

    # Output to the file
    with open( f, 'wb' ) as cfgfile:
        cfg.write(cfgfile)
    cfgfile.close()
        
 