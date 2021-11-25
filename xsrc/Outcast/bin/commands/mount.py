"""
 
Mounts a Package into the current workspace's 'xpkgs/' directory
===============================================================================
usage: orc [common-opts] mount [options] -f SPECFILE
       orc [common-opts] mount [options] <pkgname>
       orc [common-opts] mount [options] <pkgnbv>

Arguments:
    -f SPECFILE         Mounts all of the dependencies specified in the package
                        specification file 'SPECFILE'.  
    <pkgname>           Mounts the local package (specified by '<pkgname>')  
    <pkgnbv>            Mounts the package (from the Packages/ directory) 
                        specified by '<pkgnbv>'.  The format of 'pkgnbv' is: 
                        'name-branch-version', e.g. 'mypkg-main-1.1.4'
    
Options:
    -o, --override      Will cause the mount command to overwrite an previously
                        mounted package.
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    o The '-f' option does an implicit '--override' when mounting packages   
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
    
"""
import os
import symlinks
import utils
import configparser
from . import umount

from docopt.docopt import docopt

from my_globals import OUTCAST_XPKGS_DIRNAME
from my_globals import OUTCAST_XINC_DIRNAME
from my_globals import OUTCAST_LOCAL_PKG_SUFFIX
from my_globals import OUTCAST_TOP_DIR
from my_globals import OUTCAST_EXPORT_HEADERS_FNAME


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'mount', "Mounts a Package into the current workspace's 'xpkgs/' directory" ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Mount local package
    if ( args['<pkgname>'] ):
        if ( args['<pkgname>'].find('-') == -1 ):
            not_exist_msg = "ERROR: The local package - {} - does NOT exist in the current workspace.".format( args['<pkgname>'] )
            _mount_pkg( args['<pkgname>'], args['<pkgname>'], common_args['-w'], common_args, args, not_exist_msg )
            
        # Mount external package
        else:
            xpkg    = args['<pkgname>']
            pkgname = xpkg.split('-')[0]
            not_exist_msg = "ERROR: The external package - {} - does NOT exist in your global packages/ directory.".format( xpkg ) 
            _mount_pkg( pkgname, xpkg.replace('-',os.sep,2), common_args['-p'], common_args, args, not_exist_msg )
        
    # Mount external packages from package spec file
    else:
        if ( not os.path.isfile(args['-f']) ):
            exit( "ERROR: The package file - {} - does NOT exist".format( args['-f'] ) )
             
        args['--override'] = True
        _parse_spec_file( common_args, args )
 
                  

def _mount_pkg( pkgname, pkg, srcpath, common_args, args, not_exist_msg ):
    # Generate error if the package being mounted does not exist
    skip = False
    src  = srcpath + os.sep + pkg
    if ( not os.path.isdir(src) ):
        exit( not_exist_msg )
        
    # Generate a warning if the external package has already been mounted
    linkpath = common_args['-w'] + os.sep + OUTCAST_XPKGS_DIRNAME() + os.sep + pkgname
    if ( os.path.isdir(linkpath) ):
        try:
            linksrc = utils.get_pkg_source( linkpath, srcpath, common_args['-w'], OUTCAST_LOCAL_PKG_SUFFIX() )
        except OSError as ex:
            exit( ex )
            
        ppkg = linksrc
        if ( os.sep not in ppkg ):
            ppkg = ppkg + os.sep + OUTCAST_LOCAL_PKG_SUFFIX()
        prev = ppkg.replace(os.sep, ' ' )

            
        if ( not args['--override'] ):
            if ( _is_same_pkg_mount( prev, pkg, pkgname) ):
                utils.print_warning( "Skipping mount. The current package mount is the same branch/version." )
                skip = True
            else:
                exit( "ERROR: The package - {} - is already mounted ( {} ).  Mount failed".format(pkgname, ppkg) )
        else:
            utils.print_warning( "The previous package mapping ( {} ) is being overridden.".format(ppkg) )
            umount._umount_pkg( linkpath, common_args, args )
            
    # Create symbolic for the loaded package
    try:
        if ( not skip ):
            symlinks.make_link( linkpath, src ) 
            utils.print_verbose( "MOUNTED    " + linkpath + " FROM  " + src )
         
    except OSError as ex:
        exit( ex )

    # map header files
    _export_headers( src, srcpath, common_args, args )          
    

#------------------------------------------------------------------------------
def _export_headers( src, srcpath, common_args, args ):
    # Get the list of headers to export
    headers = _process_export_header_file( src )
            
    for h in headers:

        # Package is local
        if ( h.startswith(common_args['-w']) ):
            parts   = h.replace(common_args['-w'],'',1).split(os.sep)
            pkgpath = os.path.join( *parts[2:] )
            path    = os.path.join( common_args['-w'], OUTCAST_XINC_DIRNAME(), pkgpath )
        
        # Package is external
        else:
            parts   = h.replace(common_args['-p'],'',1).split(os.sep)
            pkgpath = os.path.join( *parts[4:] )
            path    = os.path.join( common_args['-w'], OUTCAST_XINC_DIRNAME(), pkgpath )

        
        skip         = False
        parent, file = os.path.split( path )
        utils.mkdirs( parent )
        
        # Generate a warning if exporting an non-existant header file
        if ( not os.path.isfile(h) ):        
            utils.print_warning( "Attempted to mount non-existed file: {}".format( h ) )
            
        # Export the file
        else:
            # Trap the file already exists in the xinc/ directory
            if ( os.path.exists(path) ):
                try:
                    linksrc = utils.get_pkg_source( path, srcpath, common_args['-w'], OUTCAST_LOCAL_PKG_SUFFIX() )
                except OSError as ex:
                    exit( ex )
            
                prev = linksrc
                if ( os.sep not in prev ):
                    prev = prev + os.sep + OUTCAST_LOCAL_PKG_SUFFIX()
                
                
                if ( not args['--override'] ):
                    if ( prev == linksrc ):
                        utils.print_warning( "Skipping mounting file: {}.  The current file is the same branch/version".format(prev) )
                        skip = True
                    else:
                        exit( "ERROR: The file - {} - is already mounted ( {} ).  Mount failed".format(pkgpath, prev) )
                else:
                    utils.print_warning( "The previous file mapping ( {} ) is being overridden.".format(prev) )
                    umount._umount_file( path, linksrc )

            # Create symoblic for the header file
            try:
                if ( not skip ):
                    symlinks.make_link( path, h )
                    utils.print_verbose( "MOUNTED    " + pkgpath )
                
            except OSError as ex:
                exit( ex )
            
            
def _process_export_header_file( pkgpath ):
    # Start with an empyt list of header files
    headers = []
    hfile   = pkgpath + os.sep + OUTCAST_TOP_DIR() + os.sep + OUTCAST_EXPORT_HEADERS_FNAME()
    
    # default to export ALL headers in the package 
    if ( not os.path.isfile(hfile) ):
        headers = utils.walk_file_list( "*.h", pkgpath + os.sep + 'src' )
        
    # Use export file when it exists
    else:
        headers = utils.read_export_header_file( hfile, pkgpath, headers )
        
    return headers



#------------------------------------------------------------------------------
def _parse_spec_file( common_args, args ):
    cfg = configparser.RawConfigParser(allow_no_value=True)
    cfg.optionxform = str
    cfg.read( args['-f'] )
            
    # load sections
    if ( cfg.has_section('immediate_deps') ):
        _load_section( cfg.options('immediate_deps'), common_args, args )
    if ( cfg.has_section('weak_deps') ):
        _load_section( cfg.options('weak_deps'), common_args, args )
    if ( cfg.has_section('transitive_deps') ):
        _load_section( cfg.options('transitive_deps'), common_args, args )
    
def _load_section( options, common_args, args ):
    for entry in options:
        entry = entry.replace('*','',1)
        pkg, branch, ver = utils.standardize_dir_sep(entry).split(os.sep)
        if ( branch == '' ):
            branch = 'main'
        xpkg = os.path.join( pkg, branch, ver )
        not_exist_msg = "ERROR: The dependent package - {} - does NOT exist in your global packages/ directory.".format( xpkg.replace(os.sep,'-') ) 
        _mount_pkg( pkg, xpkg, common_args['-p'], common_args, args, not_exist_msg )
               
    
def _is_same_pkg_mount( prev, pkg, pkgname ):
    # Trap: mounting a local package
    if ( pkg == pkgname ):
        new = os.sep + pkg + os.sep + OUTCAST_LOCAL_PKG_SUFFIX()
    else:
        new = os.sep + pkg
    
       
    # test for the same package
    new = new.replace(os.sep, ' ')
    if ( new == prev ):
        return True
    else:
        return False
           
