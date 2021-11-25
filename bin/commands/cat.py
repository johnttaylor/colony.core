"""
 
Outputs a the content of Package 'top file' to the display
===============================================================================
usage: orc [common-opts] cat [options] 
       orc [common-opts] cat [options] <pkgname>
       orc [common-opts] cat [options] <pkgnbv>

Arguments:
    <pkgname>           Package name in the current Workspace to operate on.  If 
                        no <pkgname> is specified, it is assumed that the CWD is
                        in the top/ directory of a locak package - and that
                        local package is the package to run command against.
    <pkgnbv>            Package in the Native Package Universe.  The package
                        is specified using the 'nbv' format (name-branch-
                        version, eg. 'mypkg-main-1.0.4).
                        
Options:
    --spec              Display the pkg.specification file
    --jrnl              Display the pkg.journal file
    --ns                Display the pkg.namespaces file
    --branch            Display the pkg.branching file
    --nsfilter          Display the namespaces.filter file
    --export            Display the export_headers.lst file
    --pkgjrnl           Display the package journal file (from the Native 
                        Package Universe).
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'


Notes:
    o The default operation if no file-selection option is specified is to
      display the pkg.specification file.
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
        
"""
from docopt.docopt import docopt
import subprocess, os, tarfile
import utils, deps

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'cat', 'Displays content of Package Top files.' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # Select which file to display
    file = 'pkg.specification'
    if ( args['--jrnl'] ):
        file = 'pkg.journal'
    elif ( args['--ns'] ):
        file = 'pkg.namespaces'
    elif ( args['--branch'] ):
        file = 'pkg.branching'
    elif ( args['--nsfilter'] ):
        file = 'namespaces.filter'
    elif ( args['--export'] ):
        file = 'export_headers.lst'
    
    
    # The package journal file is handle differently
    elif ( args['--pkgjrnl'] ):
        if ( not args['<pkgname>'] ):
            exit( "ERROR: When using the --pkgjrnl option you must specify a <pkgname>" )
        f = os.path.join( common_args['--uverse'], args['<pkgname>'] + ".journal" )
        _cat_file(f, "ERROR: Unable to open Package Journal file: {} ({})" )
        exit(0)
         
    # assumed top/ directory
    if ( not args['<pkgname>'] ):
        _cat_file(file, "ERROR: Unable to open Top file: {} ({})" )
 
    # local package
    elif ( args['<pkgname>'].find('-') == -1 ):
        f = os.path.join(common_args['-w'], args['<pkgname>'], 'top', file )
        _cat_file(f, "ERROR: Unable to open Top file: {} ({})" )


    # Use package from Native Uverse
    else:
        f = os.path.join( common_args['--uverse'], args['<pkgname>'] + ".top" )
        
        # Open the child top file
        try:
            tar    = tarfile.open( f )
        except Exception as ex:
            exit("ERROR: Trying to locate/read Package Top File: {} ({})".format(f,ex) )
            
        # Get the branch history
        try:
            fh = tar.extractfile( 'top/' + file )
        except Exception as ex:
            exit ( "ERROR: Package {} does not contain file: {}.".format(f,file) )
            tar.close()
            exit(0)

        # Display history
        utils.cat_file( fh, False, False )
        tar.close()    
    
    
#------------------------------------------------------------------------------
def _cat_file(f,msg):
    try:
        fd = open( f, 'r' )
        utils.cat_file( fd, False, False )
        fd.close()
            
    except Exception as ex:
        exit( msg.format(f,ex) )
        
