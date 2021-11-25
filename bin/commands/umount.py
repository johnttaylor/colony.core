"""
 
Unmounts a Package from the current workspace's 'xpkgs/' directory
===============================================================================
usage: orc [common-opts] umount -f SPECFILE
       orc [common-opts] umount <pkgname>
       orc [common-opts] umount --clean <pkgname>
       orc [common-opts] umount --clean <pkgnbv> 


Arguments:
    <pkgname>           Name of package to unmount
    -f SPECFILE         Unmounts all of the dependencies specified in the 
                        package specification file 'SPECFILE'.  
    --clean             Explicitly umounts the specified package.  When
                        just the package package name (i.e. '<pkgname>') is
                        provided, then '<pkgname>' is assumed to be local
                        package to the current Workspace.  When the package
                        name, branch, version (i.e. '<pkgnbv>') is provided,
                        the package is assumed to be from the Packages/
                        directory.
    
Options:
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    o The default operation will determine whether or not the package being 
      unmount is an external or local package.  The '--clean' options 
      should only be needed for cleaning up if a previous umount command 
      paritally completed.
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
    
"""

import os, errno, configparser
import symlinks
import utils
from docopt.docopt import docopt

from my_globals import OUTCAST_XPKGS_DIRNAME
from my_globals import OUTCAST_XINC_DIRNAME
from my_globals import OUTCAST_LOCAL_PKG_SUFFIX

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'umount', "Unmounts a Package from the current workspace's 'xpkgs/' directory" ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Use the package spec file to remove packages
    if ( args['-f'] ):
        if ( not os.path.isfile(args['-f']) ):
            exit( "ERROR: The package file - {} - does NOT exist".format( args['-f'] ) )
             
        _parse_spec_file( common_args, args )
            
    # Remove the individual package specified
    else:
        linkpath = common_args['-w'] + os.sep + OUTCAST_XPKGS_DIRNAME() + os.sep + args['<pkgname>']
        fail_on_not_mounted = True
        if ( args['--clean'] ):
            fail_on_not_mounted = False
        
        _umount_pkg( linkpath, common_args, args, fail_on_not_mounted, cleaning=True )
    

#------------------------------------------------------------------------------
def _parse_spec_file( common_args, args ):
    cfg = configparser.RawConfigParser(allow_no_value=True)
    cfg.optionxform = str
    cfg.read( args['-f'] )
            
    # load sections
    if ( cfg.has_section('immediate_deps') ):
        _unload_section( cfg.options('immediate_deps'), common_args, args )
    if ( cfg.has_section('weak_deps') ):
        _unload_section( cfg.options('weak_deps'), common_args, args )
    if ( cfg.has_section('transitive_deps') ):
        _unload_section( cfg.options('transitive_deps'), common_args, args )
    
def _unload_section( options, common_args, args ):
    for entry in options:
        entry = entry.replace('*','',1)
        pkg, branch, ver = utils.standardize_dir_sep(entry).split(os.sep)
        if ( branch == '' ):
            branch = 'main'
        xpkg = os.path.join( common_args['-w'], OUTCAST_XPKGS_DIRNAME(), pkg )
        args['<pkgname>'] = pkg + '-' + branch + '-' + ver
        args['--clean']   = True
        
        _umount_pkg( xpkg, common_args, args, False, cleaning=True )

#------------------------------------------------------------------------------
def _umount_pkg( linkpath, common_args, args, error_on_not_mounted=False, cleaning=False ):
    # Generate an error if package is not currently mounted
    if ( symlinks.is_link( linkpath ) == False and error_on_not_mounted == True ):
        exit( "ERROR: The package - {} - is not currently mounted.".format(args['<pkgname>']) )


    # Remove any/all exported header files
    xincpath = os.path.join( common_args['-w'], OUTCAST_XINC_DIRNAME() )
    linkinfo = _get_pkg_source( linkpath, common_args, args )
    
    if ( OUTCAST_LOCAL_PKG_SUFFIX() in linkinfo ):
        if ( linkinfo.startswith(os.sep) ):
            linkinfo = linkinfo[1:]
        linkinfo = os.path.join( common_args['-w'], os.path.dirname(linkinfo) )
        
    removelist = utils.walk_linksrc( linkinfo, xincpath, common_args )
    for t in removelist:
        _umount_file( t[0], t[1] )

    # Clean-up xinc/directory
    utils.walk_clean_empty_dirs(xincpath) 

    # Remove the Package link
    linkinfo = _get_pkg_source( linkpath, common_args, args )
    try:
        symlinks.remove_link( linkpath )
        utils.print_verbose( "UNMOUNTED  " +  linkinfo )
    except OSError as ex:
        if ( ex.errno == None and cleaning == True ):
            pass
        else:
            exit( ex )
    
def _umount_file( linkpath, linksrc ):
    try:
        symlinks.remove_link( linkpath )
        utils.print_verbose( "UNMOUNTED  " +  linksrc )
    except OSError as ex:
        exit( ex )

            
def _get_pkg_source( linkpath, common_args, args ):
    if ( '--clean' in args ):
        if ( args['--clean'] ):
            t = args['<pkgname>'].split('-',2)
            if ( len(t) == 1 ):
                return os.path.join( args['<pkgname>'], OUTCAST_LOCAL_PKG_SUFFIX() )
            else:
                return os.path.join( t[0], t[1], t[2] )
    
    return utils.get_pkg_source( linkpath, common_args['-p'], common_args['-w'], OUTCAST_LOCAL_PKG_SUFFIX() )      
