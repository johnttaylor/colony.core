"""
 
Checks if there are newer dependent packages available for a Package
===============================================================================
usage: orc [common-opts] newer [options] <pkgname> 
       orc [common-opts] newer [options] -f SPECFILE
       orc [common-opts] newer [options] <pkgnbv>

Arguments:
    <pkgname>           Inspect all of the dependent packages in '<pkgname>'
                        for potential newer dependent packages.  The package
                        '<pkgname>' is assumed to be a local package in the
                        current Workspace directory.
    -f SPECFILE         Path and Name of the package specification file 
                        'SPECFILE' to inspect for newer packages
    <pkgnbv>            Inspects the Native Package Universe for the package
                        specified by '<pkgnbv>'.  The format of 'pkgnbv' is:
                        'name-branch-version', e.g. 'mypkg-main-1.1.4'

Options:
    --noweak            Only inspects primary dependencies for newer packages
    --weak              Only inspects weak dependencies for newer packages
    --single            Display each newer package found as single output line.
    -h, --help          Display help for this command


Common Options:
    See 'orc --help'


Notes:
    o The default behavior is check both primary and weak dependencies for
      potential upgrades.
    o The command will fail if the current state of the package/SPECFILE is
      invalid with respect to its dependencies.
    o CAUTION: The command only compares the published time stamps.  When
      comparing publish times across branches, a newer published date does 
      NOT imply newer content and/or functionality.  
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  


Examples:
    orc newer my_pkg            
    orc newer -f pkg.specification   
    orc newer my_pkg-main-1.2.3 
                                
                                    
        
"""
from docopt.docopt import docopt
import subprocess, os
import utils, deps
from my_globals import OUTCAST_TOP_DIR

LOCAL_MARKER = '**Local**'

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'newer', 'Checks if there are newer dependent packages available.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
    
    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # PACKAGE Spec file specified
    if ( args['-f'] ):
        pkgspec = args['-f']
        p, d,w,t, cfg = deps.read_package_spec( pkgspec )
    
    elif ( args['<pkgname>'] ):
        # Get package from the Native Package Universe
        if ( args['<pkgname>'].find('-') != -1 ):
            p, d,w,t, pkgspec, cfg = _read_top_file( common_args, args['<pkgname>'] )
            
        # find spec file based on package name (local to the Workspace)
        else:
            pkgspec = os.path.join( common_args['-w'], args['<pkgname>'], OUTCAST_TOP_DIR(), "pkg.specification" )
            p, d,w,t,l, cfg = deps.read_package_spec( pkgspec )
            
         

        
    # Get dependency list
    dep_tree, act_tree = deps.validate_dependencies( p, d,w,t,l, common_args, False, pkgspec )
    
    # Drop first element in the list since it will be <pkgname>    
    packages = deps.convert_tree_to_list( act_tree )[1:]
    
    # Filter in/out weak deps 
    filtered = []
    for p in packages:
        # Skip 'locally mount' packages
        if ( LOCAL_MARKER in p ):
            continue

        if ( args['--noweak'] ):
            if ( p.startswith('*') ):
                continue
                
        elif ( args['--weak'] ):
            if ( not p.startswith('*') ):
                continue
        p = p.replace(os.sep,'-')    
        filtered.append( p if not p.startswith('*') else p[1:] )

    # Drop duplicate (there are duplicates due the nature of the tree structure/edges)    
    filtered = utils.remove_duplicates( filtered )
    
    # Get a list of all packages in the Native Uverse and convert it to a list of packages
    files = utils.get_file_list( "*.top", common_args['--uverse'] )
    pkgs  = []
    for f in files:
        p = os.path.basename(f)
        pkgs.append( (f,p) )
        
    
    # For all dependencies (in the filtered list) -->check if there are new versions available
    l = []
    for me in filtered:
            
        # Housekeeping
        depinfo = _read_top_file( common_args, me )[0]
        deptime = depinfo['pubtime']
         
        # Check against found packages
        newer   = []
        mypname = me.split('-')[0]
        for f,p in pkgs:
            if ( not p.startswith(mypname) ):
                continue
            
            # Get the publish time   
            info  = deps.read_spec_from_tar_file( f )[0]
            ptime = info['pubtime']
            if ( deptime < ptime ):
                newer.append( (p,ptime) )

        l.append( (me, deptime, newer) )        

    if ( not args['--single'] ):
        _print_pretty( l )
    else:
        _print_simple( l )
        
    
    

    

#------------------------------------------------------------------------------
def _print_pretty( l ):

    # Display dependencies that are at the latest
    flag = False
    print("Dependencies at the LATEST published package:")
    for pname,dtime,newer in l:
        if ( len(newer) == 0 ):
            flag = True
            print("{}".format(pname,dtime))
 
    # Add whitepsce between the two lists
    if ( not flag ):
        print("-->none") 
    else:
        print()

    # Display deoendencies that have newer packages
    print("Dependencies with NEWER published package(s) available:")
    flag = False
    for pname,dtime,newer in l:
        if ( len(newer) > 0 ):
            flag = True
            s = sorted(newer,key=_getKey)         
            print("{}  {}".format(pname,dtime))
            for x in s:
                print("--> {}  {}".format(x[0], x[1]))

    if ( not flag ):
        print("-->none")

def _print_simple( l ):
   for pname, dtime,newer in l:
        if ( len(newer) > 0 ):
            s = sorted(newer,key=_getKey)         
            for x in s:
                print("{} {}   {} {}".format(pname,dtime, x[0], x[1]))
            

def _read_top_file( common_args, pkg ):        
    pkgspec = os.path.join( common_args['--uverse'], pkg + ".top" )    
    p, d,w,t, cfg = deps.read_spec_from_tar_file( pkgspec )    
    return p, d,w,t, pkgspec, cfg
    

def _getKey( t ):
    return t[1]
    