"""
 
Script to checkin (and Label) one or more files to the SCM repository
===============================================================================
usage: evie [common-opts] checkin [options] <pkgname> <label> <comment> FILES...

Arguments:
    <pkgname>           Name of the package to archive.  The pkgname must be
                        a 'local package' in the current Workspace.
    <label>             Label to apply to the checked in file(s)
    <comment>           Brief comment associated with the checkin (enclosed in 
                        qoutes).
    FILES               List of files of to checkin

Options:
    --nolabel           Skips the labeling step - HOWEVER a label still must be
                        specified.
    -o, --override      Force the checkin and labeling
    -h, --help          Display help for this command

    
Notes:
    o Returns zero if all of the files where successfully checked; else non-zero 
      is returned.    
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'checkin', 'Checks in one or more files to the SCM repository.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Check if file exist yet in the repository
    for f in args['FILES']:
        t = utils.run_shell( "git ls-files --error-unmatch {}".format(f) )
        if ( t[0] != 0 ):
            utils.run_shell( "git add {}".format(f) )
            
    
    # Commit files
    cmd = 'git commit -v --author="{}" -m "{}" {}'.format( common_args['--user'], args['<comment>'], ' '.join(args['FILES']) )
    t   = utils.run_shell( cmd, common_args['-v'] )
    _check_results( t, "ERROR: Failed to commit changes." )

    # Label the commit
    force = "--force" if args['--override'] else ""
    if ( not args['--nolabel'] ):
        cmd = 'git tag {} -a  -m "{}" {}'.format( force, args['<comment>'], args['<label>'] )
        t   = utils.run_shell( cmd, common_args['-v'] )
        _check_results( t, "ERROR: Failed to create the tag/label.", clean_commit=True )
    
    # Push files
    cmd = "git push {} -v --follow-tags".format(force)
    t   = utils.run_shell( cmd, common_args['-v'] )
    _check_results( t, "ERROR: Failed to push changes.", clean_commit=True )

    

def _check_results( t, err_msg, clean_commit=False ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        if ( clean_commit == True ):
            utils.run_shell( "git reset --hard HEAD~1" )
        exit( err_msg )
