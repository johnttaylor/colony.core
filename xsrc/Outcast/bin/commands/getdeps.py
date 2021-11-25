"""
 
Pulls and Mounts dependent Packages based on a pkg.specification file
===============================================================================
usage: orc [common-opts] getdeps [options] <pkgname>
       orc [common-opts] getdeps [options] -f SPECFILE

Arguments:
    -f SPECFILE         Pulls/Mounts all of the dependencies specified in the 
                        package specification file 'SPECFILE'.  
    <pkgname>           Pulls/Mounts all of the dependencies of/for the local
                        package '<pkgname>'.
                        
Options:
    --vault FILE        Overrides the default setting of which Package Vault(s)
                        to pull the package archive from.  The default vault 
                        info is set by the Native Package Universe.
    --noweak            Do not install the package's weak dependencies.
    --weak              Install ONLY the package's weak dependencies.                        
    -o,--override       Overwrites the Pacakge Archive(s) if they already exist
                        in Packages Directory. Also overrides existing symoblic
                        links when mounting the pulled package(s)
    --rmlocal           When pulling a local package, the previous local package
                        (if it exists) will be deleted.
    -h, --help          Display help for this command

    
Notes:
    o A check of (ie. 'orc check') the dependencies is done prior to getting
      the dependent packages.  If the check fails, the get operation will be 
      aborted.
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
    
"""
import os
import utils, vault, deps
from docopt.docopt import docopt
from my_globals import OUTCAST_TOP_DIR

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'getdeps', 'Pulls and Mounts dependent Packages.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # PACKAGE Spec file specified
    if ( args['-f'] ):
        pkgspec = args['-f']
    
    # find spec file based on package name
    else:
        pkgspec = os.path.join( common_args['-w'], args['<pkgname>'], OUTCAST_TOP_DIR(), "pkg.specification" )
         

    # Get dependency list
    p, d,w,t,l, cfg = deps.read_package_spec( pkgspec )
    dep_tree, act_tree = deps.validate_dependencies( p, d,w,t,l, common_args, False, pkgspec )

    # Drop first element in the list since it will be <pkgname>
    packages = deps.convert_tree_to_list( act_tree )[1:]

    # Filter in/out weak deps 
    filtered = []
    for p in packages:
        if ( args['--noweak'] ):
            if ( p.startswith('*') ):
                continue
                
        elif ( args['--weak'] ):
            if ( not p.startswith('*') ):
                continue
            
        filtered.append( p if not p.startswith('*') else p[1:] )

    # Drop duplicate (there are duplicates due the nature of the tree structure/edges)    
    filtered = utils.remove_duplicates( filtered )

    # Housekeeping
    override  = '--override' if args['--override'] else ""
    verbose   = '-v' if common_args['-v'] else ""
    if ( not args['--vault'] ):
        args['--vault'] = os.path.join( common_args['--uverse'], "default.vault" )

    # Get the filtered dependency list
    for p in filtered:
    
        # Housekeeping
        pkg, branch, version = p.split(os.sep)
        archive = "{}-{}-{}".format( pkg, branch, version )
        
        # Using a released packages
        if ( branch != '**LOCAL**' and version != '' ):
            # Pull the package
            cmd = 'orc.py {} -w {} --user "{}" --passwd "{}" pullv --vault {} {} {}-{}-{}'.format(verbose, common_args['-w'], common_args['--user'], common_args['--passwd'], args['--vault'], override, pkg, branch, version  )
            t   = utils.run_shell( cmd, common_args['-v'] )
            _check_results( t )

    
            # Mount the package
            cmd = 'orc.py {} -w {} mount {} {}-{}-{}'.format(verbose, common_args['-w'], override, pkg, branch, version  )
            t   = utils.run_shell( cmd, common_args['-v'] )
            _check_results( t )
    
        # Using 'local' packages
        else:
            # Get the repo info for the local package. Tuple: (pkg, repo, branch, path)
            info = get_local_info(l, pkg)
            if ( info == None ):
                exit( f"Internal error - package '{pkg}' was NOT found in my local overrides list {l}" )
        
            # Set --rmlocal option
            rmopt = '--rm' if ( args['--rmlocal'] ) else ''

            # Retreive the repository
            cmd = 'evie.py {} -w {} --scm {} clone {} --repo {} {} {} {} '.format(verbose, common_args['-w'], common_args['--scm'], rmopt, info[1], info[0], info[2], info[3]  )
            t   = utils.run_shell( cmd, common_args['-v'] )
            _check_results( t )

            # Mount the local package
            cmd = 'orc.py {} -w {} mount {} {} '.format(verbose, common_args['-w'], override, pkg )
            t   = utils.run_shell( cmd, common_args['-v'] )
            _check_results( t )

#--------------------------------------------------------------------------
def get_local_info( local_list, pkg_to_find ):
    for (pkg, repo, branch, path) in local_list:
        if ( pkg == pkg_to_find ):
            return pkg, repo, branch, path
    return None


def _check_results( t ):
    if ( t[1] != None and t[1] != 'None None'):
        s = t[1].strip()
        if ( s != '' ):
            s = s.replace('ERROR: ','',1)
            s = s.replace('Warn:  ','',1)
            utils.print_warning( s )

    
