"""
 
Script performs various SCM label operations
===============================================================================
usage: evie [common-opts] label [options] delete <label>
       evie [common-opts] label [options] list [<pattern>]
       
Arguments:
    delete              Deletes the specified label
    <label>             Name of the Label to operate on
    list                List current labels
      
Options:
    <pattern>           Optional filter to applied with performing the 'list'
                        operation
    --try               Try the label operation, but do not generate an error
                        if the operation fails.
    -o, --override      Forces t.e label operation
    -h, --help          Display help for this command


Notes:
    
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'label', 'performs various SCM label operations' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # DELETE label
    if ( args['delete'] ):
        cmd = 'git tag --delete {}'.format( args['<label>'] )
        t   = utils.run_shell( cmd, common_args['-v'] )
        _check_results( args, t, "ERROR: Failed to remove/delete label: {}".format( args['<label>'] ) )

    # LIST labelse
    elif ( args['list'] ):
        pattern = args['<pattern>'] if args['<pattern>'] else ""
        cmd = 'git tag --list {}'.format( pattern)
        t   = utils.run_shell( cmd, verbose_flag=True )
        _check_results( args, t, "ERROR: Failed to retrieve label list" )
    

def _check_results( args, t, err_msg ):
    if ( not args['--try'] and t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        exit( err_msg )
