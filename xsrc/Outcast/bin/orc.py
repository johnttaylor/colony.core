#!/usr/bin/python3
"""
Orc is a Outcast tool for managing packaging and/or workspaces
===============================================================================
usage: orc [options] <command> [<args>...]
       orc [options]

Options:
    -p PPATH             Path of the packages directory (overrides the 
                         environment OUTCAST_PKGS_ROOT setting).
    -w WPATH             Explicity sets the path of the workspace root 
                         directory.  The default behavior is to auto-detect the
                         workspace root directory.
    --user USER          Set the user id (overrides the environment varaible
                         settings for the user id).
    --passwd PASS        Password for the asscoiated user id.
    --uverse UPATH       Path of the native pcakge universe directory (overrides 
                         the environment OUTCAST_PKGS_UVERSE setting).
    --utype UTYPE        Selects the type of Universe (overrides the
                         environment OUTCAST_UVERSE_TYPE setting). The default
                         Universe type is: git.
    --scm TOOL           Selects the SCM tool (overrides the environment 
                         OUTCAST_SCM_TOOL setting). The default SCM tool is:
                         git. 
    -z, --norefresh      Do not force a refresh of the Native Package Universe
                         as part of executing the command.
    --now EPOCHSEC       Explicilty provides the 'timestamp' for the distribute
                         operation.  The default is use the current time when
                         the command is executed.
    -q                   Suppresses Warning messages.
    -v                   Be verbose. 
    --where              Only display the path to where 'orc' is located.
    --qry-wpath          Displays the workspace root (i.e. the results of the
                         auto-detect workspace root).
    --qry-pkg            Display the path of the current package.
    -h, --help           Display help for common options/usage.
    

Type 'orc help <command>' for help on a specific command.
"""

import sys
import os
from subprocess import call

from docopt.docopt import docopt
from my_globals import ORC_VERSION
from my_globals import OUTCAST_SCM_TOOL
import utils




#------------------------------------------------------------------------------
def load_command( name ):
    try:
        command_module = __import__("commands.{}".format(name), fromlist=["commands"])
    except ImportError:
        exit("{} is not a Orc command. Use 'orc help' for list of commands.".format(name) )

    return command_module
        
        
#------------------------------------------------------------------------------
def display_command_list():
    import pkgutil
    import commands
    p = commands
    
    print( ' ' )
    print( "Type 'orc help <command>' for more details. Type 'orc --help' for base usage." )
    print( "-------------------------------------------------------------------------------" )
    
    for importer, modname, ispkg in pkgutil.iter_modules(p.__path__):
        if ( not ispkg ):
            cmd = load_command( modname )
            cmd.display_summary()
            

    print( ' ' )

#------------------------------------------------------------------------------
# MAIN
if __name__ == '__main__':
    # Parse command line
    args = docopt(__doc__, version=ORC_VERSION(), options_first=True )

    # Determine which SCM tool to use
    scm = os.environ.get( OUTCAST_SCM_TOOL() )
    if ( scm == None ):
        scm = 'git'
    if ( args['--scm'] ):
        scm = args['--scm']    
    args['--scm'] = scm

    # Trap the special where option
    if ( args['--where'] ):
        print(__file__)
        exit(0)
 
    # Trap help on a specific command
    if ( args['<command>'] == 'help' ):

        # Display list of commands if none specified
        if ( args['<args>'] == [] ):
            display_command_list()
        
        # Display command specific help
        else:
            load_command( args['<args>'][0] ).run( args, ['--help'] )


    # Trap no command specified        
    elif ( args['<command>'] == None and not args['--qry-wpath'] and not args['--qry-pkg'] ):
            docopt(__doc__,argv=['--help'])
    

    # Run the command (if it exists)
    else:
        # Set quite & verbose modes
        utils.set_quite_mode( args['-q'] )
        utils.set_verbose_mode( args['-v'] )
        skip = False
    
        # Handle the expection case(s) for needing the environment variables being set
        skipenv = False
        if ( args['<command>'] == 'set' ):
            skipenv = True

        # Handle the expection case(s) for needing to know the workspace/ root
        skipwrk = False
        if ( args['<command>'] == 'mkwrk' ):
            skipwrk = True

        # Housekeeping
        utils.set_user_name( args, skipenv )
        utils.set_password( args )
        utils.set_workspace( args, skipwrk )
        utils.set_uverse( args, skipenv )
        utils.set_packages_dir( args, skipenv )

        # Trap the special --qry-wpath option
        if ( args['--qry-wpath'] ):
            print(args['-w'])
            exit(0)
        
        # Trap the special --qry-pkg option
        if ( args['--qry-pkg'] ):
            cwd  = os.path.realpath( os.getcwd() ) 
            here = cwd.replace(args['-w']+os.sep,'')
            pkg  = here.split(os.sep)[0]
            print(args['-w'] + os.sep + pkg)
            exit(0)
        
        
        # run the command
        load_command( args['<command>'] ).run( args, [args['<command>']] + args['<args>'] )


    
