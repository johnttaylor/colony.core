"""Collection of helper functions related to package dependencies"""

import configparser, os, tarfile
import utils
import tempfile
import shutil

LOCAL_MARKER = "**Local**"

#------------------------------------------------------------------------------
def validate_dependencies( pkginfo, list_deps, list_weak, list_trans, list_local, common_args, nocheck_flag, file_name, cache=None ):

    # check for duplicate names in a package file
    if ( not nocheck_flag ):
        _valid_no_duplicate_pkgs( list_deps, list_weak, list_trans, list_local, file_name ) 
    
    # Housekeeping
    t         = (pkginfo['name'], pkginfo['branch'], pkginfo['version'])
    trail     = []
    all       = []
    immediate = []
    root      = utils.Node( t )
    all.extend( list_deps )
    all.extend( list_weak )
    if ( cache == None ):
        cache = {}
        
    # calculate all of the possible transitive dependencies
    trail.append( (file_name, t[0], t[1], t[2]) )   
    build_node( root, all, list_local, common_args['--uverse'], trail, cache, list_weak )

    # Skip most of the test(s) when --nocheck
    root_actual = utils.Node( t )
    if ( not nocheck_flag ):
        # Housekeeping for: verify listed transitive dependencies are valid
        allall    = list(all)
        allall.extend( list_trans )
        
        # check against "do-not-use" packages
        check_against_blacklist( common_args['--uverse'], allall, common_args, warn_on_do_not_use="Initial check shows a dependency on a 'Do-Not-Use' Package: {}-{}-{}." )

        # Run algorithm
        _build_actual( root_actual, all, allall, list_local, common_args['--uverse'], trail, cache, list_weak )

        # Check for stale transitive dependencies
        stale = find_unused_dependencies( root_actual, list_trans )
        if ( len(stale) > 0 ):
            print( "ERROR: Stale dependencies in the [transitive_deps] section:" )
            for d in stale:
                print(( "ERROR:   " + d ))
            exit(1)
                
        # Clean-up tree for print/display purposes
        trail  = [t]
        remove_duplicate_children( root_actual, trail )
    
        # Find weak ONLY dependencies
        trail = [t]
        root_noweak = utils.Node(t)
        build_node( root_noweak, list_deps, list_local, common_args['--uverse'], trail, cache, None )
        root_actual_noweak = utils.Node(t)
        _build_actual( root_actual_noweak, list_deps, allall, list_local, common_args['--uverse'], trail, cache, list_weak )
            
        # update returned trees with weak-ONLY info
        _mark_weak_only_nodes( root, root_noweak )
        _mark_weak_only_nodes( root_actual, root_actual_noweak )
        
        # Do another check against blacklist packages now that stale transitive dependencies have been removed
        check_against_blacklist( common_args['--uverse'], allall, common_args )
        
    # Return final trees                 
    return root, root_actual



def check_against_blacklist( uverse, list_all_deps, common_args, warn_on_do_not_use=None ):
    # check my dependencies against the "black list"
    for d in list_all_deps:

        # Read the dependency's publish histroy
        pkgname = d[0]
        fname   = os.path.join( uverse, pkgname ) + ".journal"
        journal = utils.read_journal_file( fname )
    
        # Check for hard errors
        l = utils.extract_pbv_from_journal( journal, 'do_not_use', pkgname )
        if ( d in l ):
            if ( warn_on_do_not_use != None ):
                utils.print_warning( warn_on_do_not_use.format( d[0], d[1], d[2] ) ) 
            else:
                exit( "ERROR: Dependent package - {}-{}-{} - has been marked as DO NOT USE.  You MUST update your dependencies to proceed.".format( d[0], d[1], d[2] ) )
        
        # Check for warning
        l = utils.extract_pbv_from_journal( journal, 'deprecate', pkgname )
        if ( d in l and warn_on_do_not_use == None):
            utils.print_warning( "Dependent package - {}-{}-{} - has been deprecated.  Recommend you upgrade the package.".format( d[0], d[1], d[2] ) )
        

            
def convert_tree_to_list( tree ):
    s      = encode_dep( tree.get_data(), False, False )
    result = [s]
    for cnode in tree.get_children():
        cdata = cnode.get_data()
        result.extend( convert_tree_to_list( cnode ) )
        
    return result
    
    
#------------------------------------------------------------------------------
def _mark_weak_only_nodes( full_tree, noweak_tree ):
    noweak_list = convert_tree_to_list( noweak_tree ) 
    _mark_tree( full_tree, noweak_list )
    

def _mark_tree( node, noweak_list ):
    for child in node.get_children():
        data  = child.get_data()
        entry = encode_dep( data, False, False )
        if ( not entry in noweak_list ):
            newval = ('*'+data[0], data[1], data[2] )
            child.set_data( newval )
        _mark_tree( child, noweak_list )


def _check_results( t, err_msg ):
    if ( t[0] != 0 ):
        if ( t[1] != None and t[1] != 'None None' ):
            print(t[1])
        exit( err_msg )

#------------------------------------------------------------------------------
def find_unused_dependencies( tree, list_of_deps, encode=True ):

    # Convert the actual tree to a SET
    trans = set()
    edges = flatten_to_edges( tree )
    for x in edges:
        l,r = x.split(" : ")
        l   = _strip_and_expand( l )
        r   = _strip_and_expand( r )
        trans.add(l)
        trans.add(r)

    unused = []    
    for y in list_of_deps:
        i = encode_dep( y, False, False )
        if ( not i in trans ):
            if ( encode ):
                unused.append( i )
            else:
                unused.append( y )
            
    return unused
    
#------------------------------------------------------------------------------
def remove_duplicate_children( node, seen ):
    for cnode in node.get_children():
        cdata = cnode.get_data()
        if ( cdata in seen ):
            cnode.remove_all_children_nodes()
            continue
        
        seen.append( cdata )    
        remove_duplicate_children( cnode, seen )
            
#------------------------------------------------------------------------------
def read_package_spec( filename, top_fname='', fh=None ):

    cfg = configparser.RawConfigParser(allow_no_value=True,delimiters=('='))
    cfg.optionxform = str
    fname           = filename
    
    if ( top_fname == '' ):
        if ( os.path.isfile(filename) ):
            utils.print_verbose( "Reading file: " + filename )
            cfg.read( filename )
        else:
            exit( "ERROR: File - {} - does not exist or is not a valid file name.".format( filename ) )
                
    else:
        utils.print_verbose( "Reading file: "  + top_fname  )
        fname = top_fname
        d = fh.read()
        fh.close()
        if ( 'bytes' in str(type(d)) ):
            d = d.decode("utf-8")
        cfg.read_string( d )
    
    list_deps  = []
    list_weak  = []
    list_trans = []
    list_local = []
    
    # load dependencys sections
    if ( cfg.has_section('immediate_deps') ):
        list_deps = _load_section( 'immediate_deps', cfg, fname )
        
    if ( cfg.has_section('weak_deps') ):
        list_weak = _load_section( 'weak_deps', cfg, fname )

    if ( cfg.has_section('transitive_deps') ):
        list_trans = _load_section( 'transitive_deps', cfg, fname )

    if ( cfg.has_section('local_overrides') ):
        list_local = _load_override_section( 'local_overrides', cfg, fname )

    # load package info
    pkginfo = read_info(cfg, filename, top_fname)
    return pkginfo, list_deps, list_weak, list_trans, list_local, cfg 
    


def read_spec_from_tar_file( f ):
    # Open the child top file
    try:
        tar = tarfile.open( f , mode="r:*")
        fh  = tar.extractfile( 'top/pkg.specification' )

        ## Make a temporary file that 'supports Unicode reads' (the tar file reads are 'str' not bytes/byte tuples)
        fp = tempfile.TemporaryFile(mode="w+b")
        d = fh.read()
        fh.close()
        fp.write( d )
        fp.seek(0)

    except Exception as ex:
        exit(f"ERROR: Trying to locate/read Package Top File: {f} ({ex})" )
    
    # Get pkg.specification info from the top file
    pkginfo  = read_package_spec( 'pkg.specification', f, fp )
    fp.close();
    tar.close()
    return pkginfo


#------------------------------------------------------------------------------
def build_node( parent_node, children_data, list_local, path_to_uverse, trail, cache, weak_deps, no_warn_on_weakcycle=False, trans=None ):
    parent_node.add_children_data( children_data )
    override_pkgs = _extract_pkgnames( list_local )
    for cnode in parent_node.get_children():
        # Skip local overrides
        pkg,b,v = cnode.get_data()
        if ( pkg in override_pkgs ):
            cnode.set_data( (pkg, LOCAL_MARKER, '') )
            continue

        p,b,v = _get_node_data_and_substitute( cnode, trans )
        fname = build_top_fname(p,b,v)
        cpath = os.path.join( path_to_uverse, fname )

        
        # trap weak-cyclic dependencies
        n = (fname, p, b, v)
        if ( _test_for_weak_cyclic(n,trail,weak_deps) ):
            if ( not no_warn_on_weakcycle ):
                utils.print_warning( "Weak Cyclic dependency." )
                utils.print_warning( "   " + _convert_trail_to_string(trail,n) )
            parent_node.remove_child_node( cnode ) 
            continue
            
        # trap cyclic dependencies
        if ( _test_for_cyclic(n,trail) ):
            print("ERROR: Cyclic dependency.")
            _display_trail(trail,n)
            exit(1)

        # Keep a trail for diagnostic/error messages purposes
        trail.append(n)

        # Read the child's top file (but ONLY once)
        info = read_top_file( cpath, fname, trail, cache )
        
        # Process the child top file
        i,d,w,t,bhist = info
        if ( len(d) > 0 ):
            build_node( cnode, d, list_local, path_to_uverse, trail, cache, weak_deps, no_warn_on_weakcycle, trans )
            
        trail.remove(n)
    
            
def _get_node_data_and_substitute( node, trans=None ):
    # default behavior
    if ( trans == None ):
        return node.get_data()
    
    # substitute different version for a transitive dependency
    p,b,v = node.get_data()
    for x in trans:
        if ( p == x[0] ):
            node.set_data( x )
            return x
    
    # If I get here than no substitution is needed/required        
    return (p,b,v)
   
        
def build_top_fname( p, b, v ):
    return p + '-' + b + '-' + v + '.top'
       
         
def _test_for_cyclic( t, trail ):
    for x in trail:
        if ( t[1] == x[1] ):
            return True
    
    return False


def _test_for_weak_cyclic( t, trail, weak_deps ):
    if ( weak_deps != None and len(trail) > 1 ):
        root           = trail[0]
        imm_child      = trail[1]
        weak_pkg_names = _extract_pkgnames( weak_deps )
        
        if ( imm_child[1] in weak_pkg_names ):
            if ( t[1] == root[1] ):
                return True

    return False;


def _display_trail(trail, n=None):
    print("ERROR:   ", end=' ')
    print(_convert_trail_to_string(trail,n))

        
def _convert_trail_to_string( trail, n=None):
    result = []
    for i in trail:
        result.append( encode_dep(i[1:],use_quotes=False) )
        result.append( " -> " )
    if ( n != None ):
        result.append( encode_dep(i[1:],use_quotes=False) )
        
    return "".join(result)
    

#------------------------------------------------------------------------------
def _load_section( section, cfg, fname ):
    children = []
    options  = cfg.options(section)
    
    for entry in options:
        entry = entry.replace('*','',1)
        try:
            pkg, branch, ver = utils.standardize_dir_sep(entry).split(os.sep)
        except:
            exit( "ERROR: Malformed dependency entry [{}.{}] in file: {}".format(section,entry,fname) )
            
        if ( branch == '' ):
            branch = 'main'
        children.append( (pkg, branch, ver) )

    return children       
    
def _load_override_section( section, cfg, fname, sep=';' ):
    children = []
    options  = cfg.options(section)
    
    for entry in options:
        entry = entry.replace(' ','')
        try:
            pkg, repo, branch, path = entry.split(sep)
        except:
            exit( "ERROR: Malformed dependency entry [{}.{}] in file: {} (must be: pkg;repo;branch;path)".format(section,entry,fname) )
        children.append( (pkg, repo, branch, path) )

    return children       

#------------------------------------------------------------------------------
def _valid_no_duplicate_pkgs( list_deps, list_weak, list_trans, list_local, fname ):
    
    # check by section
    _valid_no_duplicate_pkgs_in_section( list_deps,  fname, "Duplicate Package name(s) within section: [immediate_deps]" )
    _valid_no_duplicate_pkgs_in_section( list_weak,  fname, "Duplicate Package name(s) within section: [weak_deps]" )
    _valid_no_duplicate_pkgs_in_section( list_trans, fname, "Duplicate Package name(s) within section: [transitive_deps]" )
    _valid_no_duplicate_pkgs_in_section( list_local, fname, "Duplicate Package name(s) within section: [local_overrides]" )

    # check for dups across sections
    l = []
    l.extend( list_deps )
    l.extend( list_weak )
    _valid_no_duplicate_pkgs_in_section( l, fname, "Duplicate Package name(s) occurred in section: [weak_deps]" )
    l.extend( list_trans )
    _valid_no_duplicate_pkgs_in_section( l, fname, "Duplicate Package name(s) occurred in section: [transitive_deps]" )

        
def _valid_no_duplicate_pkgs_in_section( list_deps, fname, msg ):
    pnames     = _extract_pkgnames( list_deps )
    duplicates = utils.find_duplicates_in_list( pnames )
    if ( len(duplicates) > 0 ):
        print("ERROR: In file - " + fname + ' - ' + msg) 
        for n in duplicates:
            print("ERROR:   " + n)
        exit(1)        
    
def _extract_pkgnames( list_of_deps ):
    list = []
    for v in list_of_deps:
        list.append(v[0].lower())
        
    return list     


#------------------------------------------------------------------------------
def create_dot_file( fname, root ):
    with open(fname, 'w' ) as f:
        f.write( "digraph { \n" )
        
        for item in flatten_to_edges( root ):
            item = item.replace(os.sep,"/")
            f.write( item.replace(" : "," -> ",1) + "\n" )
            
        f.write( "}\n" )

def _write_node( f, parent_node ):
    parent = encode_dep( parent_node.get_data() )
    
    for cnode in parent_node.get_children():
        child = encode_dep( cnode.get_data() )
        f.write( parent + " -> " + child + "\n")
        _write_node( f, cnode )

#------------------------------------------------------------------------------
def flatten_to_edges( root, compress_flag=True ):
    s = set()
    _flatten_node( s, root, compress_flag )
    return list(s)
    
def _flatten_node( flatset, parent_node, compress_flag ):
    parent = encode_dep( parent_node.get_data(), compress_flag )
    for cnode in parent_node.get_children():
        child = encode_dep( cnode.get_data(), compress_flag )
        edge  = parent + " : " + child
        flatset.add( edge )
        _flatten_node( flatset, cnode, compress_flag )
        
def encode_dep(t,compress_flag=True, use_quotes=True):
    p,b,v = t
    q     = '"'
    if ( use_quotes == False ):
        q = ''
        
    if ( compress_flag == True and b == 'main' ):
        b = ''
    return '{}{}{}{}{}{}{}'.format( q, p,os.sep, b,os.sep, v, q )
    
        
def _strip_and_expand( pkg ):
    pkg = pkg.strip('"')
    return pkg.replace(os.sep+os.sep, os.sep + "main" + os.sep, 1)


def tree_as_a_list( root ):
    l = []
    _walk_tree( root, l )
    return l
  
def _walk_tree( node, list ):
    list.append( node.get_data() )
    for cnode in node.get_children():
        list.append( cnode.get_data() )
        _walk_tree( cnode, list )
          
#------------------------------------------------------------------------------
def _read_string_value( pkginfo, cfg, section, key, filename, topfname, default_value=None ):
    if ( not cfg.has_option(section,key) ):
        exit( f"ERROR: Missing [{section}.{key}] option in file:{filename} (in top file:{topfname})" )
    pkginfo[key] = default_value if ( cfg.get(section,key ) == None or cfg.get(section,key ) == '' ) else  cfg.get(section,key )
     
def _read_int_value( pkginfo, cfg, section, key, filename, topfname, default_value=None ):
    if ( not cfg.has_option(section,key) ):
        exit( f"ERROR: Missing [{section}.{key}] option in file:{filename} (in top file:{topfname})" )
    pkginfo[key] = default_value if ( cfg.get(section,key ) == None or cfg.get(section,key ) == '' ) else  int(cfg.get(section,key ))

def read_info( cfg, filename, top_fname ):
    
    if ( not cfg.has_section('info') ):
        exit( "ERROR: Missing [info] section in file:{} (in top file:{})".format(filename, top_fname) )

    pkginfo = {}    
    _read_string_value( pkginfo, cfg, 'info','name', filename, top_fname )
    _read_string_value( pkginfo, cfg, 'info','branch', filename, top_fname )
    _read_string_value( pkginfo, cfg, 'info','version', filename, top_fname )
    _read_int_value( pkginfo, cfg, 'info','pubtime', filename, top_fname )
    _read_string_value( pkginfo, cfg, 'info','pubtimelocal', filename, top_fname )
    _read_string_value( pkginfo, cfg, 'info','desc', filename, top_fname )
    _read_string_value( pkginfo, cfg, 'info','owner', filename, top_fname )
    _read_string_value( pkginfo, cfg, 'info','email', filename, top_fname )
    _read_string_value( pkginfo, cfg, 'info','url', filename, top_fname )

    return pkginfo
    
        
#------------------------------------------------------------------------------
def _build_actual( node, children_data, all_entries, list_local, path_to_uverse, trail, cache, weak_deps ):
    override_pkgs = _extract_pkgnames( list_local )
    node.add_children_data( children_data )
    for cnode in node.get_children():
        p,b,v = cdata = cnode.get_data()

        # Skip over local-overrides
        if ( p in override_pkgs ):
            cnode.set_data( (p, LOCAL_MARKER, '') )
            continue

        # Trap missing transitive dependency (and properly handle the weak-cyclic condition)
        entry = _find_pkg_in_list( p, all_entries )
        if ( entry == None ):
            if ( not _test_for_weak_cyclic( ("",p,b,v), trail, weak_deps ) ):
                print(("ERROR: Missing a transitive dependency: {}".format(encode_dep(cdata,use_quotes=False)) ))
                _display_trail(trail)
                exit(1)
            else:
                node.remove_child_node( cnode ) 
                continue
            
        # Housekeeping
        pa,ba,va      = entry
        fname         = build_top_fname(pa,ba,va)
        cpath         = os.path.join( path_to_uverse, fname )
        a             = (fname, pa,ba,va)
        trail.append(a)

        # Access the child node's TOP file - including the branching history
        i,d,w,t,bhist = read_top_file( cpath, fname, trail, cache )
     
        # Check if actual transitive dependency is valid
        if ( not utils.is_ver1_backwards_compatible_with_ver2(entry,cdata,bhist) ):
            print(("ERROR: Actual transitive dependency is NOT compatible. Required min ver={}".format(encode_dep(cdata,use_quotes=False)) ))
            _display_trail(trail)
            exit(1)
       
        # modify child node to reflex actual transitive dependency
        cnode.set_data( entry )
        
        # process the child
        if ( len(d) > 0 ):
            _build_actual( cnode, d, all_entries, list_local, path_to_uverse, trail, cache, weak_deps )
        trail.remove(a)


    
    
def _find_pkg_in_list( pkg, pkglist ):
    for e in pkglist:
        p,b,v = e
        if ( pkg.lower() == p.lower() ):
            return e
    return None

def read_top_file( cpath, fname, trail, cache ):
    if ( not fname in cache ):
        # Open the child top file
        try:
            tar = tarfile.open( cpath )
            fh  = tar.extractfile( 'top/pkg.specification' )
       
        except Exception as ex:
            print(("ERROR: Trying to locate/read Package Top File: {} ({})".format(cpath, str(ex)) ))
            _display_trail(trail)
            exit(1)

        # Make a temporary file that supports Unicode reads because the configParser only works with Unicode strings 
        with open("deps.tmp", "wb" ) as fp:
            fp.write( fh.read() )
            fp.seek(0)

            # Get child dependency data from the top file
            with open( "deps.tmp" ) as fp:
                info, d,w,t,l, cfg = read_package_spec( 'pkg.specification', fname, fp )
                bhist            = _get_branching_history( tar, trail )
                cache[fname]     = (info,d,w,t,bhist)
        os.remove( "deps.tmp" ) #fp.close();
        tar.close()
        _valid_no_duplicate_pkgs( d, w, t, l, fname ) 

    return cache[fname]
    

    

def _get_branching_history( tar, trail ):

    # Open the branch history file
    try:
        fh  = tar.extractfile( 'top/pkg.branching' )
  
    # If no pkg.branching file is present -->then the assumption is that there is no branch history
    except KeyError:
        return None
    
    # Some other error -->hard stop    
    except Exception as ex:
        print(("ERROR: Cannot open the 'pkg.branching' file in Package Top File: {}".format(cpath) ))
        _display_trail(trail)
        exit(1)
    
    # Get the branch history
    n = utils.parse_branch_log( fh )
    fh.close()
    return n[0]
    
    
    