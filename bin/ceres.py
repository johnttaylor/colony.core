#! /usr/bin/env python3
"""
 
Ceres is a Outcast tool for managing Outcast Universes
===============================================================================
usage: ceres [options] <command> [<args>...]
       ceres [--qry]
       ceres [--help]

Options:
    --user USER          Set the user id (overrides the environment varaible
                         settings for the user id).
    --passwd PASS        Password for the asscoiated user id                         
    -w WPATH             Explicity sets the path of the workspace root 
                         directory.  The default behavior is to auto-detect the
                         workspace root directory.
    --uverse UPATH       Path of the package universe directory (overrides the
                         environment OUTCAST_PKGS_UVERSE setting).
    --utype UTYPE        Selects the type of Universe (overrides the
                         environment OUTCAST_UVERSE_TYPE setting). The default
                         Universe type is: git.
    --now EPOCHSEC       Explicilty provides the 'timestamp' for the distribute
                         operation.  The default is use the current time when
                         the command is executed
    --qry                List the support Universe types.
    -q                   Suppresses Warning messages
    -v                   Be verbose 
    -h, --help           Display help for common options/usage
    

Type 'ceres help <command>' for help on a specific command.

"""

import sys
import os
from subprocess import call

from docopt.docopt import docopt
import utils

from my_globals import CERES_VERSION
from my_globals import OUTCAST_UVERSE_TYPE



#------------------------------------------------------------------------------
def load_command( utype, name ):
        try:
            command_module = __import__("uverse_types.{}.{}".format(utype, name), fromlist=[utype])
        except ImportError:
            exit("%r is not a Ceres command. Use 'ceres help' for list of commands." % name)

        return command_module
        
        
#------------------------------------------------------------------------------
def display_command_list(utype):
    import pkgutil
    p = __import__("uverse_types.git",fromlist=['git'] )
    
    print( ' ' )
    print( "Type 'ceres help <command>' for details. Type 'ceres --help' for base usage." )
    print( "-------------------------------------------------------------------------------" )
    for importer, modname, ispkg in pkgutil.iter_modules(p.__path__):
        if ( not ispkg ):
            cmd = load_command( utype, modname )
            cmd.display_summary()
            

    print( ' ' )

def display_uverse_types_list():
    print( ' ' )
    print( "Type 'ceres --help' for additional help." )
    print( "-------------------------------------------------------------------------------" )
    bpath = os.path.join( os.path.dirname(__file__), 'uverse_types' )
    if ( os.path.exists( bpath ) ):
        files = os.listdir(bpath)
        for f in files:
            if ( os.path.isdir(os.path.join(bpath,f)) ):
                print(f)
      
    print( ' ' )

#------------------------------------------------------------------------------
# Parse command line
args = docopt(__doc__, version=CERES_VERSION(), options_first=True )

# Display list of build engines supported
if ( args['--qry'] ):
    display_uverse_types_list()

else:
    # Determine the type of universe, aka which command set to use
    utype = os.environ.get( OUTCAST_UVERSE_TYPE() )
    if ( utype == None ):
        utype = 'git'
    if ( args['--utype'] ):
        utype = args['--utype']    

    # Trap help on a specific command
    if ( args['<command>'] == 'help' ):

        # Display list of commands if none specified
        if ( args['<args>'] == [] ):
            display_command_list(utype)
        
        # Display command specific help
        else:
            load_command( utype, args['<args>'][0] ).run( args, ['--help'] )


    # Trap no command specified        
    elif ( args['<command>'] == None ):
            docopt(__doc__,argv=['--help'])
    

    # Run the command (if it exists)
    else:
        # Set quite & verbose modes
        utils.set_quite_mode( args['-q'] )
        utils.set_verbose_mode( args['-v'] )
    
    
        # Handle the expection case(s) for needing the environment variables being set
        skipenv = False
        if ( args['<command>'] == 'set' ):
            skipenv = True

        # Housekeeping
        utils.set_user_name( args, skipenv )
        utils.set_password( args )
        utils.set_uverse( args, skipenv )
        utils.set_workspace( args )
        
        # run the command
        load_command( utype, args['<command>'] ).run( args, [args['<command>']] + args['<args>'] )


    