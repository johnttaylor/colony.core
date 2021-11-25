"""
 
Checks a pkg.specification file for valid dependencies
===============================================================================
usage: orc [common-opts] checkdeps [options] 
       orc [common-opts] checkdeps [options] <pkgname>

Arguments:
    <pkgname>           Package name in the current Workspace to check.  If no 
                        <pkgname> is specified, it is assumed that the CWD is
                        in the top/ directory of a locak package - and that
                        local package is the package run checkdeps on.

Options:
    --dot               Generate two '.dot' that can be rendered by GraphViz.
    --graph FORMAT      Converts the GraphViz '.dot' files to the specified
                        document FORMAT.  Example 'formats': "pdf", "svg"
                        Note: the '--graph' option does an implicit '--dot'
    --print             Display the package dependency trees on the console
    --nocheck           Skips most error checking and produces just the '.dot'
                        files.  Note: Some error checking can not be bypassed,
                        e.g. cyclic dependency check.
    --compare           Generates the 'actual' dependency tree and 
                        compares it against what is currently mounted
                        in the local workspace.  The command returns non
                        zero if one or more mounted dependent packages
                        do meet the 'mimimum versions'. Note: Local
                        package overrides are ASSUMED to meet the required
                        mimimum version(s).
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'


Notes:
    o Two '.dot' files are created: 'derived' and 'actual'.  The 'derived' 
      tree contains all dependencies as defined by the package's 1st level 
      dependencies and all of the subsequent dependencies as defined by the
      transitive packages' dependencies.  The 'actual' tree contains the 
      dependencies as enumerated solely by package's 'pkg.specification' file.
    o A prefix of '*' on a package name indicates that the package is a weak
      dependency or is transitive of only weak dependencies.
    o The version numbers show on the 'dervied' graph is the minimum required
      vesion.  The 'actual' graph shows what version where installed when the
      package was pushed.
    o For more details about GraphViz, see http://www.graphviz.org/
    o The 'compare' operation will fail (i.e return non-zero) if there are 
      local packages mounted AND there are no matching 'local-override' 
      directive(s) in the package's specification file.
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
        
"""
from docopt.docopt import docopt
import subprocess, os
import utils, deps

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'checkdeps', 'Checks a pkg.specification file for valid dependencies.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
    
    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Local to the package's top/ directory
    file = 'pkg.specification'

    # Explicity local package name     
    if ( args['<pkgname>'] ):
        path = os.path.join( common_args['-w'], args['<pkgname>'], 'top' )
        if ( not os.path.isdir( path ) ):
            exit( "ERROR: Invalid <pkgname> ({})".format( path ) )
        utils.push_dir( path )
        
    # Ensure the Native Package universe is current
    if ( not common_args['--norefresh'] ):
        cmd = 'ceres.py -v -w {} --user "{}" --passwd "{}" refresh'.format(common_args['-w'], common_args['--user'], common_args['--passwd'] )
        t   = utils.run_shell( cmd, common_args['-v'] )
        _check_results( t, "ERROR: Failed Refresh the Native Package Universe." )


    p, d,w,t,l, cfg = deps.read_package_spec( file )
    dep_tree, act_tree = deps.validate_dependencies( p, d,w,t,l, common_args, args['--nocheck'], file )
    if ( not args['--nocheck'] ):
        print("All dependency checks PASSED.")
    else:
        print("SKIPPED one or more dependency checks.")
         
    # Compare operation
    if ( args['--compare'] ):
        compare_workspace( dep_tree, common_args['-v'], args['<pkgname>'] );

    # Other options...
    else:
        # generate .DOT file
        if ( args['--dot'] or args['--graph'] ):
            _create_dot_file( file, "derived", dep_tree, args['--graph'] )
            _create_dot_file( file, "actual", act_tree, args['--graph'] )


        if ( args['--print'] ):
            print("Derived Transitive tree.")
            print(dep_tree)
            if ( not args['--nocheck'] ):
                print("Actual Package tree.")
                print(act_tree)

    
    
    
    
#------------------------------------------------------------------------------
def _create_dot_file( file, qualifier, tree, graph_opt ):
    # create .DOT file
    oname = file + "." + qualifier + ".dot"
    deps.create_dot_file( oname, tree )
    
    # generate the visual graph as a XXX file
    utils.render_dot_file_as_pic( graph_opt, oname )


#------------------------------------------------------------------------------
def _check_results( t, err_msg ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        exit( err_msg )

#------------------------------------------------------------------------------
def compare_workspace( dep_tree, verbose, pkg ):
    # Pull the package
    cmd = f'orc.py ls'
    t   = utils.run_shell( cmd, verbose )
    _check_results( t, "ERROR: Failed to get the list of currently mounted packages" )
    mounted = t[1].splitlines()
    
    # Convert my dep tree into something more useful
    dlist = []
    utils.flatten_tree_to_list( dep_tree, dlist )

    # Compare my device to the mounted packages
    success = True
    for p,b,v in dlist:
        # Remove leading '*' for weak dep marker
        pn = p[1:] if ( p[0] == '*' ) else p

        # Is package mounted?
        mnt = get_from_mounted_list( mounted, pn )
        if ( mnt == None ):
            print( f"NO PACKAGE mounted for: ({p}, {b}, {v})" );
            success = False
            continue

        #print("DBG", b,mnt)
        # Is local mount AND there is override specified?
        if ( b == "**Local**" ): 
            if ( mnt[1] == "**Local**" ): 
                print( f"({p}, **Local**)" );
                continue
        
            # Local mount -->but no override
            else:
                print( f"MISSING LOCAL mount for: ({p})" );
                success = False
                continue
        elif ( mnt[1] == "**Local**" ): 
            print( f"UN-EXPECTED LOCAL mount mounted for: ({p}, {b}, {v})" );
            success = False
            continue                    

        # Is the mounted version compatible?
        if ( utils.is_ver1_backwards_compatible_with_ver2( (pn,b,v), mnt ) ):
            print( f"({p}, {b}, {v})" );
        else:
            print( f"NO COMPATIBLE Pkg mounted for: ({p}, {b}, {v}) [mounted: {mnt[0]}{os.sep}{mnt[1]}{os.sep}{mnt[2]}]" );
            success = False

    if ( not success ):
        exit( "Compare failed!" )
    else:
        print("The workspace matches the Package's dependency configuration")


def get_from_mounted_list( mlist, pkg_to_find ):

    for i in mlist:
        p = i.split(os.sep)
        if ( p[0] == pkg_to_find ):
            return p
    return None