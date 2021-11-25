"""
 
Locks one or more (for exclusive access) Native Package metadata files
===============================================================================
usage: ceres [common-opts] lock [options] FILES... 
       ceres [common-opts] lock [options] --unlock FILES...

Arguments:
    FILES               One or more metadata files in the Native Package
                        Universe to lock for exclusive access.  Any path
                        information is relative to the root of the Native
                        Package Universe
    --unlock            Removes the lock from the specified files.  Note: The
                        'ceres commit' file does an implict removal of the 
                        lock.
                                            
Options:
    -h, --help          Display help for this command

    
Notes:
    o The --unlock operation has the side effect REVERTING any/all changes made
      the metadata files. 
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'lock', 'Locks one or more Native Package metadata files' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # GIT does have the concept of global/remote lock -->so for now I put my head in 
    # in the sand and hope we don't two simultaneous users trying to lock/mod the
    # same metafile(s)!
    # TODO: Fix me or do not use GIT for Package Universes!
    
    # Housekeeping
    utils.push_dir( common_args['--uverse'] )
    for f in args['FILES']:
        if ( not os.path.isfile(f) ):
            exit( "ERROR: {} is not valid metadata file.".format(f) )
    
    # Unlock, aka revert changes
    if ( args['--unlock'] ):
        cmd = 'evie.py -v -w {} --scm git revert --use-cwd notused {}'.format( common_args['-w'], ' '.join(args['FILES']) )
        utils.run_shell( cmd, common_args['-v'] )
        print("Lock removed and changes reverted for files: {}".format( ' '.join(args['FILES']) ))
        
    # Lock, aka do nothing
    else:
        print("Files locked: {}".format( ' '.join(args['FILES']) ))
        
    # Clean-up
    utils.pop_dir()
        
        
            