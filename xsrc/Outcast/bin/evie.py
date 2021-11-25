#! /usr/bin/env python3
"""
 
Evie is an Outcast wrapper for invoking your SCM tool
===============================================================================
usage: evie [options] <command> [<args>...]
       evie [--qry]
       evie [--help]

Options:
    --user USER          Set the user id (overrides the environment varaible
                         settings for the user id).
    --passwd PASS        Password for the asscoiated user id
    -w WPATH             Explicity sets the path of the workspace root 
                         directory.  The default behavior is to auto-detect the
                         workspace root directory.
    --scm TOOL           Selects the SCM tool (overrides the environment 
                         OUTCAST_SCM_TOOL setting). The default SCM tool is:
                         git 
    --now EPOCHSEC       Explicilty provides the 'timestamp' for the distribute
                         operation.  The default is use the current time when
                         the command is executed
    --qry                List the supported SCM tools.
    -q                   Suppresses Warning messages
    -v                   Be verbose 
    -h, --help           Display help for common options/usage
    

Type 'evie help <command>' for help on a specific command.

"""

import sys
import os
from subprocess import call

from docopt.docopt import docopt
import utils

from my_globals import EVIE_VERSION
from my_globals import OUTCAST_SCM_TOOL



#------------------------------------------------------------------------------
def load_command( scm, name ):
    try:
        command_module = __import__("scm.{}.{}".format(scm, name), fromlist=[scm])
    except ImportError:
        exit("%r is not a Evie command. Use 'evie help' for list of commands." % name)
    
    return command_module
        
        
#------------------------------------------------------------------------------
def display_command_list(scm):
    import pkgutil
    p = __import__("scm.{}".format(scm),fromlist=['git'] )
    
    print( ' ' )
    print( "Type 'evie help <command>' for details. Type 'evie --help' for base usage." )
    print( "-------------------------------------------------------------------------------" )
    for importer, modname, ispkg in pkgutil.iter_modules(p.__path__):
        if ( not ispkg ):
            cmd = load_command( scm, modname )
            cmd.display_summary()
            

    print( ' ' )

def display_scm_types_list():
    print( ' ' )
    print( "Type 'evie --help' for additional help." )
    print( "-------------------------------------------------------------------------------" )
    bpath = os.path.join( os.path.dirname(__file__), 'scm' )
    if ( os.path.exists( bpath ) ):
        files = os.listdir(bpath)
        for f in files:
            if ( os.path.isdir(os.path.join(bpath,f)) ):
                print(f)
      
    print( ' ' )

#------------------------------------------------------------------------------
# Parse command line
args = docopt(__doc__, version=EVIE_VERSION(), options_first=True )

# Display list of build engines supported
if ( args['--qry'] ):
    display_scm_types_list()

else:
    # Determine which SCM tool to use
    scm = os.environ.get( OUTCAST_SCM_TOOL() )
    if ( scm == None ):
        scm = 'git'
    if ( args['--scm'] ):
        scm = args['--scm']    
    args['--scm'] = scm

    # Trap help on a specific command
    if ( args['<command>'] == 'help' ):

        # Display list of commands if none specified
        if ( args['<args>'] == [] ):
            display_command_list(scm)
        
        # Display command specific help
        else:
            load_command( scm, args['<args>'][0] ).run( args, ['--help'] )


    # Trap no command specified        
    elif ( args['<command>'] == None ):
        docopt(__doc__,argv=['--help'])
    

    # Run the command (if it exists)
    else:
        # Set quite & verbose modes
        utils.set_quite_mode( args['-q'] )
        utils.set_verbose_mode( args['-v'] )
    
        # Housekeeping
        utils.set_user_name( args )
        utils.set_password( args )
        utils.set_workspace( args )
 
        # run the command
        load_command( scm, args['<command>'] ).run( args, [args['<command>']] + args['<args>'] )


    