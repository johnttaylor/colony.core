"""
 
Distributes the Package Top file to the Native and Foriegn Package Universes
===============================================================================
usage: ceres [common-opts] distribute [options] <pkgtop> <comment>
       ceres [common-opts] distribute --revert <pkgtop>

Arguments:
    <pkgtop>            Path and name of the local Package Top File to add the 
                        Native Package universe.
    <comment>           Comment associated with the action.
    --revert            Used to clean-up after failed distribute command
                                              
Options:
    -o, --override      Overrides the top file (if it previously exists) in 
                        Native Universe with the file specified.
    -h, --help          Display help for this command

    
Notes:
    o The command also updates the package's metedata file to log/record the
      publish event.    
    
    
"""
import os, shutil
import utils
from docopt.docopt import docopt

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'distribute', 'Distributes the Package Top file to Package Universes.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Housekeeping
    global top, pkg, root, jrnl
    pkg   = os.path.basename(common_args['--uverse'])
    root  = os.path.dirname(common_args['--uverse'])
    top   = os.path.basename( args['<pkgtop>'] )
    p,e   = os.path.splitext(top)
    n,b,v = p.split('-',2)
    jrnl  = n + ".journal"
    global dst
    dst = os.path.join( common_args['--uverse'], top )


    # REVERT
    if ( args['--revert'] ):
        _clean_up( True )
        exit(0)

    # Capture publish time
    now, local = utils.mark_time(common_args['--now'])

    # Check if there are any pending changes in the Native Uverse - if there is this is BAD because that means someone is manually messing with the Package Universes!
    utils.push_dir( common_args['--uverse'] )
    cmd = 'evie.py -v -w {} --user "{}" --passwd "{}" --scm git pending {}'.format( root, common_args['--user'], common_args['--passwd'], pkg )
    t   = utils.run_shell( cmd, common_args['-v'] )
    utils.pop_dir()
    _check_results( t, "ERROR: Failed pending check." )
    

    # Copy the file to the Native Uverse
    try:
        if ( os.path.isfile(dst) ):
            if ( args['--override'] ):
                utils.print_warning( "Overwriting top file ({}) in the Native Package universe.".format( os.path.basename(dst) ) )
                os.remove(dst)
            else:
                exit( "ERROR: Top file - {} - already exists in the Native Package universe.".format(  os.path.basename(dst) ) )
        shutil.move( args['<pkgtop>'], dst )
    except Exception as ex:
        exit( "ERROR: {}".format(ex) )
    
    # Update the Package's journal file
    utils.push_dir( common_args['--uverse'] )
    utils.update_journal_publish( jrnl, common_args['--user'], args['<comment>'], False, v, b, "n/a" )
    
        
    # Checkin the new file which will ALSO push the commit to the foreign pkg uverse
    cmd = 'evie.py -v -w {} --user "{}" --passwd "{}" --scm git checkin --nolabel {} {} "{}" {} {}'.format(root, common_args['--user'], common_args['--passwd'], pkg, 'notused', args['<comment>'], top, jrnl )
    t   = utils.run_shell( cmd, common_args['-v'] )
    _check_results( t, "ERROR: Failed to update Native and Forgien Package Universes with Top file: {}.".format(top), revert=True )
    utils.pop_dir()
        
        

def _clean_up(revert=False):
    global dst
    if ( os.path.isfile(dst) ):
        os.remove( dst )
    if ( revert ):
        global top, pkg, root, jrnl
        cmd = 'evie.py -v -w {} --scm git revert {} {} {}'.format( root, pkg, top, jrnl )
        utils.run_shell( cmd )
                
            
def _check_results( t, err_msg, revert=False ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        _clean_up(revert)
        exit( err_msg )
