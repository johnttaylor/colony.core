"""
 
Provides operations to check for namespace collisions in the current workspace
===============================================================================
usage: orc [common-opts] namespaces [options] --list 
       orc [common-opts] namespaces [options] --list <pkgname>
       orc [common-opts] namespaces [options] --list <pkgnbv>
       orc [common-opts] namespaces [options] --check 
       orc [common-opts] namespaces [options] --check <pkgname> <pkgnbv>
       orc [common-opts] namespaces [options] --update <pkgname>

Arguments:
    --list              List the contents of specified package's pkg.namespaces
                        file. If no package argument is provide then the 
                        namespacess for all local package in the current 
                        Workspace is listed.
    --check             When no additional arguments are provided, checks all 
                        packages in the current Workspace for any namespace 
                        collisions. The optional arguments will check for 
                        namespace collision between the local package <pkgname> 
                        and the Native Universe package <pkgnbv>.
    --update            Updates the specified package's pkg.namespaces file
                        using Outcast's default algorithm of directories map
                        to namespaces.
    <pkgname>           Package name in the current Workspace.                   
    <pkgnbv>            Package in the Native Package Universe.  The package
                        is specified using the 'nbv' format (name-branch-
                        version, eg. 'mypkg-main-1.0.4).
      
Options:
    --print             Displays the update namespaces (only applicable when 
                        specifying the 'update' sub-command).
    -h, --help          Display help for this command



Common Options:
    See 'orc --help'
    
    
Notes:
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    o The default Outcast algorithm for automatically detecting namespace is
      based on the assumption that any non-empty directory under the src/
      directory is a namespace with the following exceptions/rules:
         - The 'namespaces.filter' file in the package's top/ directory can be
           used to specify files and directories that are EXCLUDED when 
           making the determination that a directory is empty.  For example:
           if the file name 'README.txt' is listed in the namespaces.filter
           file than if a directory only contains the file 'README.txt' then
           that directory is considered empty.  If a directory is specified
           in namespaces.filter file than that directory is ommit from the
           list of directories crawled when searching the src/ directory
           tree.
         - Format of the namespaces.file file:
            o Blank lines or lines starting with '#' are skipped
            o F|D:<name_with_wildcards> is used to specify file names or
              directories names at each sub-directory LEVEL to be ommitted in 
              the crawl.
            o T:<name>[</child>...] omit a specific directory tree from the
              crawl
            o I:<namespace> is used to unconditional always include <namespace>
              in the final updated list of namespaces.
            o All names are assumed/treated as case sensitive.
            
            Examples:
            -------------------------------------------------------------------
            # Omit all README files
            F:README*  
            
            # Omit all directories that start with '_'
            D:_*

            # Exclude a sub tree (relative to the src/ directory
            T:Foo/Bar
            
            # Always include the Bob::Your::Uncle namespace in the final output
            I:Bobs::Your::Uncle
            -------------------------------------------------------------------
    
"""
import os, fnmatch, tarfile
import utils
from docopt.docopt import docopt
from my_globals import OUTCAST_XINC_DIRNAME
from my_globals import OUTCAST_XPKGS_DIRNAME

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'namespaces', 'Check for namespace collisions in the current workspace' )) 
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Trap the '.' notation for <pkgname> argument
    utils.check_use_current_package( args )

    # LIST sub-command
    if ( args['--list'] ): 
    
        # Display all local namespaces
        if ( not args['<pkgname>'] ):
            # Get all of the namespaces in the Workspace
            all, pkgsall = utils.workspace_namespaces_as_list( common_args['-w'] ) 
            for pkg,ns in pkgsall:
                for n in ns:
                    print("{}  {}".format(pkg,n)) 
        
        # local package
        elif ( args['<pkgname>'].find('-') == -1 ):
            f = os.path.join(common_args['-w'], args['<pkgname>'], 'top', 'pkg.namespaces' )
            try:
                fd = open( f, 'r' )
                utils.cat_file( fd )
                fd.close()
                    
            except Exception as ex:
                exit( "ERROR: Unable to open namespace file: {} ({})".format(f,ex) )

        # Use package from Native Uverse
        else:
            f = os.path.join( common_args['--uverse'], args['<pkgname>'] + ".top" )
            
            # Open the child top file
            try:
                tar    = tarfile.open( f )
            except Exception as ex:
                exit("ERROR: Trying to locate/read Package Top File: {} ({})".format(f,ex) )
                
            # Get the namespaces list
            try:
                fh = tar.extractfile( 'top/pkg.namespaces' )
                
            except Exception as ex:
                exit ( "ERROR: Package {} does not contain a pkg.namespaces file.".format(f) )
                tar.close()
                exit(0)
 
            # Display history
            utils.cat_file( fh )
            tar.close()    
    
        
    # UPDATE sub-command
    elif ( args['--update'] ):
        ffilter, dfilter, tfilter, always = _read_filter( common_args['-w'], args['<pkgname>'] ) 
        path       = os.path.join(common_args['-w'], args['<pkgname>'], 'src' )    
        namespaces = []
        
        # Walk src/ tree
        for root, dirs, files in os.walk(path):
            # skip the src/ directory
            if ( root == path ):
                continue
            
            # Filter out files/dirs    
            for pattern in ffilter:
                for f in list(files):
                    if ( fnmatch.fnmatch(f,pattern) ):
                        files.remove(f)

            # if there are any files left -->then declare this directory a 'namespace'
            if ( len(files) > 0 ):
                namespaces.append( utils.standardize_dir_sep(root.replace(path+os.sep,'')) )
                
            # Filter out directories
            for pattern in dfilter:
                for d in list(dirs):
                    if ( fnmatch.fnmatch(d,pattern) ):
                        dirs.remove(d)

        # Filter out explicity sub-dir trees
        for t in tfilter:
            utils.remove_from_list( t, namespaces )                           

            
        # Update the namespaces.filter file with the results
        f = os.path.join(common_args['-w'], args['<pkgname>'], 'top', 'pkg.namespaces' )
        try:
            fd = open( f, 'wb' )

            # Add my 'always' list
            for n in always:
                fd.write( n + "\n" )
                if ( args['--print'] ):
                    print(n)

            # Add my derived list
            for n in namespaces:
                n = n.replace(os.sep,'::')
                fd.write( n + "\n" )
                if ( args['--print'] ):
                    print(n)

            fd.close()

        except Exception as ex:
            exit( "ERROR: Unable to open namespace file: {} ({})".format(f,ex) )

    # CHECK sub-command
    elif ( args['--check'] ):

        # Check the Workspace
        if ( not args['<pkgname>'] ):
            # Get all of the namespaces in the Workspace
            all, pkgsall = utils.workspace_namespaces_as_list( common_args['-w'] ) 
                                   
            # Find duplicates
            print( "all", all )
            dups = utils.find_duplicates_in_list( all )
            if ( len(dups) == 0 ):
                print("No namespace collisions.")
                exit(0)
            
            # Display offending packages
            for p,ns in pkgsall:
                for n in ns:
                    if ( n in dups ):
                        print("{}  {}".format(p,n)) 
            exit(1)
              
        # Compare two packages
        else:
            # Get the namespace info                
            local  = utils.read_namespaces_local( common_args['-w'], args['<pkgname>'] )
            uverse = utils.read_namespaces_top( common_args['--uverse'], args['<pkgnbv>'] )

            # Find duplicates
            all = local
            all.extend( uverse )
            dups = utils.find_duplicates_in_list( all )
            if ( len(dups) == 0 ):
                print("No namespace collisions.")
                exit(0)
            
            # display duplicate namespaces    
            for d in dups:
                print(d)
            exit(1)    
    
                
#------------------------------------------------------------------------------
    
def _read_filter( wrkspace, pkgname ):
    files  = []
    dirs   = []
    trees  = []
    always = [] 
    # Load filter file (if it exists )
    f = os.path.join( wrkspace, pkgname, 'top', 'namespaces.filter' )
    if ( os.path.isfile( f ) ):
        # Parse filter file    
        try:
            fd = open( f, 'r' )
            for line in fd:
                line = line.strip()
                if ( line.startswith('#') ):
                    continue
                if ( line == '' ):
                    continue
            
                if ( line.startswith(("F:","f:")) ):
                    files.append(line[2:] )
                elif ( line.startswith(("D:","d:")) ):
                    dirs.append( line[2:] )
                elif ( line.startswith(("T:","t:")) ):
                    trees.append( utils.standardize_dir_sep(line[2:]) )
                elif ( line.startswith(("I:","i:")) ):
                    always.append( utils.standardize_dir_sep(line[2:]) )
                else:
                    utils.print_warning( "namespaces.filter file: Invalid entry [{}]".format(line) )

            fd.close()
                
        except Exception as ex:
            exit( "ERROR: Unable to open/read namespaces.filter file: {})".format(f,ex) )
    

    return files, dirs, trees, always


