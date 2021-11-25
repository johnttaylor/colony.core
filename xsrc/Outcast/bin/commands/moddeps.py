"""
 
Adds, updates, and deletes a Package's dependencies
===============================================================================
usage: orc [common-opts] moddeps [options] <pkgname> ACT-PKG...
       orc [common-opts] moddeps [options] -f PKGSPEC ACT-PKG...


Argments:
    <pkgname>           Name of the Package to have its dependencies modified.
                        The 'pkgname' must a local Package to the current
                        Workspace
    -f PKGSPEC          Name and path of the pkg.specification file to update 
                        and/or modify.
    ACT-PKG             One or more 'action-package' pairs. An action-package
                        pair specifies an individual package (and version) and
                        how to apply it to the Package's dependencies.  See
                        below for format/details of action-package pair.
    
Options:
    --dot               Generate two '.dot' that can be rendered by GraphViz.
    --graph FORMAT      Converts the GraphViz '.dot' files to the specified
                        document FORMAT.  Example 'formats': "pdf", "svg"
                        Note: the '--graph' option does an implicit '--dot'
    --print             Display the resultant package dependency trees on the 
                        console
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'


Notes:
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  


Action-Package Pair:
    The 'action' portion of the Action-Package pair specifies what type of
    dependency is being modified and how that dependency should be modified.
    Packages are specified use 'nbv' format, e.g. 'my_pkg-main-2.01'.  The
    format for an action-package is: <action>:<pkg>
    
    Valid Actions:
        immediate-add       - Add a new immedidate dependency
        immediate-update    - Change the version/branch of an existing 
                              immedidate dependency.
        weak-add            - Add a new weak dependency
        weak-update         - Change the verison/branch of an existing weak
                              dependecny
        transitive-update   - Change the version/branch of an existing 
                              transitive dependency
        delete-immediate    - Removes an existing immedidate dependency
        delete-weak         - Removes an existing weak dependency
        
        
    Notes:
      o Actions are transitive in nature, i.e. they have a cascade effect. This 
        means 'downstream' dependencies will be modified/remove as defined by
        the dependency requirements of the the 'upstream' packages.
      o If the result of the depdencies changes result in an inconsistent or
        invalid set of dependencies than the command will fail and no
        changes to the Packages dependencies will be made.
      o It recommended to modify many dependencies using a single call to this
        command.  This is due to the potential relationships between the 
        dependencies being updated - it is not always possible to update the 
        Packages one-at-time due to the checking/validation of the resultant 
        dependency tree. 
      o Actions can be abbrievated to single letter (see examples).  Interally
        there are only four actions: immediate, weak, transitive, and delete.
        The verbose syntax is for readability/clarity only.
      o When using the a 'delete-*' action, only the package name is required,
        i.e. the branch/version number can be omitted.
 
    Examples:
        // Updates existing weak dependency for the 'nqbp' package
        // and adds a new immedidate dependency on package 'a'
        orc moddeps mypkg weak-update:nqbp-main-0.3.1 i:a-main-1.0.1

        // Adds, updates, and delete multiple dependencies
        orc moddeps mypkg imm:b-main-1.3.0 transitive:b-main-2.0.5 del:nqbp
        
"""
from docopt.docopt import docopt
import subprocess, os
import utils, deps
from my_globals import OUTCAST_TOP_DIR

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'moddeps', "Adds, updates, and deletes a Package's dependencies" ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
    
    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )
    
    # Get pkg.specification file to operate on
    if ( args['-f'] ):
        pkg_spec = args['-f']
    else:
        pkg_spec  = os.path.join( common_args['-w'], args['<pkgname>'], OUTCAST_TOP_DIR(), "pkg.specification" )

    # Parse action pairs
    immediate = []
    weak      = []
    trans     = []
    deldeps   = []
    for pair in args['ACT-PKG']:
        act,pkg = pair.split(':')
        t  = tuple(pkg.split('-',2))
        
        # Handle delete action as a special case, i.e. only package name is required
        if ( "delete-immediate".startswith(act) or "delete-weak".startswith(act) ):
            deldeps.append( t[0] )

        else:
            if ( len(t) != 3 ):
                exit( "ERROR: Invalid package ({}:{})".format( act, pkg ) )
            p,b,v   = t
        
            if ( "immediate-add".startswith(act) or "immediate-update".startswith(act) ):
                immediate.append( (p,b,v) )
            elif ( "weak-add".startswith(act) or "immedidate-update".startswith(act) ):
                weak.append( (p,b,v) )
            elif ( "transitive-update".startswith(act) ):
                trans.append( (p,b,v) )
            elif ( "delete-immediate".startswith(act) or "delete-weak".startswith(act) ):
                deldeps.append( (p,b,v) )
            else:
                exit( "ERROR: Invalid action ({}:{})".format( act,pkg ) )
            
            
    # Read current dependency tree
    me_info, me_d,me_w,me_t, me_cfg = deps.read_package_spec( pkg_spec )
    
    # Validate transitive updatas
    for x in trans:
        for y in me_t:
            if ( x[0] == y[0] ):
                break
        else:
            exit( "ERROR: Invalid and/or non-transitive package specified for the 'transitive-update' action." )
                      
    # apply DELETE actions
    for pkg_to_del in deldeps:
        if ( not _remove_from_list( pkg_to_del, me_d ) ):
            if ( not _remove_from_list( pkg_to_del, me_w ) ):
                exit( "ERROR: Attempting to deleting a immediate/weak dependency that does not exist: {}".format( pkg_to_del ) )

    # apply Immediate and Weak Actions
    _modify_direct_list( me_d, immediate )
    _modify_direct_list( me_w, weak )
        
    # Ensure the Native Package universe is current
    if ( not common_args['--norefresh'] ):
        cmd = 'ceres.py -v -w {} --user "{}" --passwd "{}" refresh'.format(common_args['-w'], common_args['--user'], common_args['--passwd'] )
        t   = utils.run_shell( cmd, common_args['-v'] )
        _check_results( t, "ERROR: Failed Refresh the Native Package Universe." )


    # Derive the Package's new dependency tree
    cache = {}
    t     = (me_info['name'], me_info['branch'], me_info['version'])
    trail = []
    all   = []
    root  = utils.Node( t )
    all.extend( me_d )
    all.extend( me_w )
    trail.append( (pkg_spec, t[0], t[1], t[2]) )
    deps.build_node( root, all, common_args['--uverse'], trail, cache, me_w, True, trans )
    
    # Attemp to resolve dependency conflicts
    if ( not _reconcile_conflicts( root, common_args['--uverse'], cache ) ):
        print("ERROR: final derived dependency tree")
        print(root)
        exit( "ERROR: Unable to resolve the new dependency tree" )

    # Create new transitive list
    for direct_child in root.get_children():
        for transitive in direct_child.get_children():
            trans.extend( deps.tree_as_a_list(transitive) )
    temp_trans = utils.remove_duplicates(trans)          
   
    # Remove myself/(aka the root) from the transitive list (this will happen when there is a weak-cyclic scenario)
    trans = []
    for n in temp_trans:
        if ( n[0] != t[0] ):
            trans.append(n)
        
    # Remove transitive dependencies that are also direct dependencies (aka immedidate or weak deps)
    for direct in all:
        utils.remove_from_list( direct, trans )

        
    # Validate new dependency tree
    dep_tree, act_tree = deps.validate_dependencies( me_info, me_d,me_w,trans, common_args, False, pkg_spec, cache)

    # Generate 'marked' transitive list
    _marked_weak_trans_deps( trans, act_tree )

            
    # Save changes to the pkg.specification file
    _update_section( me_cfg, "immediate_deps", me_d )
    _update_section( me_cfg, "weak_deps", me_w )
    _update_section( me_cfg, "transitive_deps", trans )

    with open( pkg_spec, 'w' ) as cfgfile:
        me_cfg.write(cfgfile)
        cfgfile.close()

    
    # generate .DOT file
    fname = os.path.basename( pkg_spec )
    if ( args['--dot'] or args['--graph'] ):
        _create_dot_file( fname, "derived", dep_tree, args['--graph'] )
        _create_dot_file( fname, "actual", act_tree, args['--graph'] )

    
    if ( args['--print'] ):
        print("Derived Transitive tree.")
        print(dep_tree)
        print("Actual Package tree.")
        print(act_tree)


#------------------------------------------------------------------------------
def _update_section( cfg, section, dep_list ):
    cfg.remove_section(section)
    cfg.add_section(section)
    for d in dep_list:
        cfg.set(section, deps.encode_dep(d,True,False) )


def _marked_weak_trans_deps( trans_list, node ):
    _mark_if_weak( node.get_data(), trans_list )
    for n in node.get_children():
        _mark_if_weak( n.get_data(), trans_list )
        _marked_weak_trans_deps( trans_list, n )

def _mark_if_weak( t, trans_list ):
    p,b,v = t
    if ( p.startswith('*') ):
        pp = p[1:]
        if ( (pp,b,v) in trans_list ):
            idx = trans_list.index((pp,b,v))
            trans_list[idx] = (p,b,v)
            
            
#------------------------------------------------------------------------------
def _modify_direct_list( dst_list, src_list ):

    # Add OR replace the 'new' package
    for t in src_list:
        p,b,v = t
        for p2,b2,v2 in dst_list:
            if ( p == p2 ):
                dst_list.remove( (p2,b2,v2) )
                break

        dst_list.append( t )
                 

def _remove_from_list( name, list ):
    for t in list:
        if ( name == t[0] ):
            list.remove( t )
            return True
    return False            


#------------------------------------------------------------------------------
def _create_dot_file( pkg_spec, qualifier, tree, graph_opt ):
    # create .DOT file
    oname = pkg_spec + "." + qualifier + ".dot"
    deps.create_dot_file( oname, tree )
    
    # generate the visual graph as a XXX file
    utils.render_dot_file_as_pic( graph_opt, oname )



#------------------------------------------------------------------------------
def _reconcile_conflicts( root, uverse, cache ):

    # Housekeeping
    maxcount = 100       
    count    = 0 
    loop     = True
    
    # Iterate until there are no conflicting packages
    while( count < maxcount ):
        count += 1
        utils.print_verbose( "Reconcile conflicts pass# {}.".format( count ) )
 
        # Get next conflict
        packages = {}
        p        = _find_next_lowest_dup_package( root, packages )
        if ( p == None ):
            break;
            
             
        # Get the most compatible package from the list of duplicates
        pbv, compnode = _find_compatible_package( root, packages, p, cache, uverse )

        # Replace all duplicates with found compatible package
        dups = packages[p]
        for p,b,v,node in dups:
            if ( (p,b,v) == pbv ):
                continue

            # replace duplicate node AND its subtree/children nodes!
            node.clone_sub_tree_from( compnode )            
            
           
    # Return the status of the reconciliation
    return True if count < maxcount else False
    
    
def _find_compatible_package( root, packages, p, cache, uverse ):

    # Iterate through all 'duplicate' packages
    lowest  = 99999
    lownode = None
    lowpbv  = None
    dups    = packages[p]
    for p,b,v,node in dups:
        info   = _read_top_file( (p,b,v), uverse, cache )
        newest = True
        
        # Trap the lowest (i.e. closest to the root) duplicate package
        if ( node.get_nodenum() < lowest ):
            lowest  = node.get_nodenum()
            lownode = node
            lowpbv  = (p,b,v)
              
        for p2,b2,v2,node2 in dups:
            if ( (p,b,v) != (p2,b2,v2) ):
                if ( not utils.is_ver1_backwards_compatible_with_ver2((p,b,v),(p2,b2,v2),info[4]) ):
                    newest = False
                    break
                    
        # Exit my search if I have found the largest/newest backwards compatible package for the list of duplicates    
        if ( newest ):
            utils.print_verbose( "  Compatible version found for package: {} ({}-{}-{}).".format( p, p,b,v ) )
            return ((p,b,v),node)
            
    # If I get here the duplicates span a major version index -->to use the lowest package
    utils.print_verbose( "  NO compatible version found for package: {}, using {}-{}-{}.".format( p, p,b,v ) )
    return (lowpbv,lownode)
                      

    
def _find_next_lowest_dup_package( tree, packages ):

    # label each based on its relationship to the root (aka a breath first traversal)
    tree.set_node_number_by_height( 1 )

    # Create a dictionary of all referenced packages
    _add_tree_to_dict( packages, tree )

    duplicates = False
    lowlevel   = 99999
    lowpkg     = None
    for k,v in packages.items():

        # Individual package exists more than once - but is it really different?
        if ( len(v) > 1 ):

            # Loop through all entries for the same package
            item = v[0]
            for x in v:
                # If the branch OR version are different -->than the package IS different
                if ( item[1] != x[1] or item[2] != x[2] ):
                    duplicates = True
                                        
                    # Capture the 'lowest level' entry for this pacakge
                    if ( item[3].get_nodenum() < x[3].get_nodenum() ):
                        if ( item[3].get_nodenum() < lowlevel ):
                            lowlevel = item[3].get_nodenum()
                            lowpkg   = item[0] 
                    else:
                        if ( x[3].get_nodenum() < lowlevel ):
                            lowlevel = x[3].get_nodenum()
                            lowpkg   = x[0] 


    # Return the lowest (i.e. closest to the root) package that is in the tree more than once with different branch/version#
    if ( duplicates ):
        utils.print_verbose( "  Lowest 'duplicate' package found: {} (node# {}).".format( lowpkg, lowlevel ) )
        return lowpkg
    else:
        utils.print_verbose( "  No (or no additional) 'duplicate' packages found." )
        return None

     
#------------------------------------------------------------------------------
def _add_tree_to_dict( d, node ):
    _add_to_dict( d, node )
    for x in node.get_children():
        _add_tree_to_dict( d, x )


def _add_to_dict( d, node ):
    p,b,v = node.get_data()
    if ( p not in d ):
        d[p] = list()
  
    d[p].append( (p,b,v,node) )


def _read_top_file( pbv, uverse, cache ):
    # Housekeeping
    pa,ba,va      = pbv
    fname         = deps.build_top_fname(pa,ba,va)
    cpath         = os.path.join( uverse, fname )
    a             = (fname, pa,ba,va)
    trail         = []
    trail.append(a)
    
    # Access the child node's TOP file - including the branching history
    return deps.read_top_file( cpath, fname, trail, cache )
 

#------------------------------------------------------------------------------
def _check_results( t, err_msg ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        exit( err_msg )
                