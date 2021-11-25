"""
 
Creates a package top file for a local package
===============================================================================
usage: orc [common-opts] top [options] 
       orc [common-opts] top [options] -k PKGNAME
       orc [common-opts] top [options] -t TOPPATH

Arguments:
    -k PKGNAME          Creates a 'package top' file for the local package
                        named 'PKGNAME'.  
    -t TOPPATH          Path do the a package's 'top/' directory, i.e. create a 
                        'top' file for a package that is not in the local 
                        Workspace directory tree.
                         
Options:
    -o, --override      Will overwrite the package top file if it already exists
    --format FMT        Sets the compressor format to FMT [Default: tgz]
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    o The default behavior (when no '-k' option is specified) create the 'top'
      file for current package.  The default behavior ONLY works if your
      current working directory is within a local package directory tree.
    o The output 'top' file is place in the root of the current Workspace
      directory.
    
    
"""
import os
import shutil
from docopt.docopt import docopt
import utils, deps
from my_globals import OUTCAST_TOP_DIR


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'top', 'Creates a package top file for a local package.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
 
    # Trap the -t option
    if ( args['-t'] ):
        dir_to_tar = args['-t']
        
    # Trap the -k option
    elif ( args['-k'] ):
        dir_to_tar = args['-k']
        dir_to_tar = os.path.join( common_args['-w'], dir_to_tar, OUTCAST_TOP_DIR() )

    # Tar the current local package dir        
    else:
        dir_to_tar = os.getcwd()
        dir_to_tar = dir_to_tar.replace(common_args['-w'],'',1)
        dir_to_tar = os.path.split(dir_to_tar)[0].replace(os.sep,'',1)
        dir_to_tar = os.path.join( common_args['-w'], dir_to_tar, OUTCAST_TOP_DIR() )
        
    # Check if source dir exists
    if ( not os.path.isdir(dir_to_tar) ):
        exit( "ERROR: Directory - {} - does not exist".format(dir_to_tar) )        

    # Extract the info for naming the output file
    pkginfo,x,y,z = deps.read_package_spec( os.path.join(dir_to_tar, "pkg.specification") )
    outname       = pkginfo['name'] + '-' + pkginfo['branch'] + '-' + pkginfo['version'] + '.top'
    toptar        = os.path.join( common_args['-w'], outname )
            
    # Create tar file
    utils.push_dir( os.path.dirname(dir_to_tar) )
    override  = '--override' if args['--override'] else ""
    cmd = 'evie.py -v -w {} --user "{}" --passwd "{}" archive {} --format {} -d {} {} {} '.format(common_args['-w'], common_args['--user'], common_args['--passwd'], override, args['--format'], OUTCAST_TOP_DIR(), pkginfo['name'], toptar )
    t   = utils.run_shell( cmd, common_args['-v'] )
    _check_results( t, "ERROR: Failed Archiving package top/ directory." )
    if ( common_args['-v'] ):
        utils.print_tar_list( toptar )
    utils.pop_dir()  
    
def _check_results( t, err_msg ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        exit( err_msg )
