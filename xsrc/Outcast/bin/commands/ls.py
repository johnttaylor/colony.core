"""
 
Lists the mounted packages and available packages
===============================================================================
usage: orc [common-opts] ls [options] [<filter>]

Arguments:
    <filter>            Optional wildcard/filter
      
Options:
    -k, --packages      List availabe packages in the packages/ directory. The
                        default behavior is to list the packages currently 
                        loaded in the Workspace.
    -n, --native        Lists available packages in the Native Package universe. 
    --short             List only Package names (does NOT apply when using the 
                        '--native' or '--remote' options).

    -h, --help          Display help for this command

Options only available for use with '--native':
    --long              Displays the Packages' publish time
    --newer DATETIME    Filters the list to packages published on or later than
                        'DATETIME'.
    --older DATETIME    Filters the list to packages published on or before
                        'DATATIME'   

Common Options:
    See 'orc --help'
    
    
Notes:
    o The 'DATETIME' argument can be specified in two ways:
      1) Full name of package (name-branch-version).  In this case the publish
         date of the specified package is used for comparison date
      2) A specific date/time with the format of: "YYYY/MM/DD HH:MM:SS" where 
         time is 24hr clock and the 'DATETIME' argument must be enclosed within 
         quotes. The entered time should be the user's local time.  The command 
         will provide the neccesary conversation between local time and UTC 
         time. 
         

Examples:
    orc ls
    orc ls --packages
    orc ls --native --long --older "2014/09/15 22:02:38" 
    orc ls --native --long --newer y-main-1.3.4
    
    
    
"""
import os, fnmatch
import symlinks, deps
import utils
from docopt.docopt import docopt

from my_globals import OUTCAST_XPKGS_DIRNAME
from my_globals import OUTCAST_XINC_DIRNAME
from my_globals import OUTCAST_LOCAL_PKG_SUFFIX

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'ls', 'Lists the mounted packages and available packages.' )) 
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Housekeeping
    filter = '*'
    if ( args['<filter>'] ):
        filter = args['<filter>']
    
        
    # List Packages in the packages/ directory
    if ( args['--packages'] ):
        pkgs = []
 
        # Short listing
        if ( args['--short'] ):
            parent0 = common_args['-p']
            for g0 in next(os.walk(common_args['-p']))[1]:

                # Apply filter
                if ( fnmatch.fnmatch( g0, filter ) ):
                    print(g0)

        # Full listing
        else:
            parent0 = common_args['-p']
            for g0 in next(os.walk(common_args['-p']))[1]:
                parent1 = parent0 + os.sep + g0
                for g1 in next(os.walk(parent1))[1]:

                    # Process leaf
                    parent2 = parent1 + os.sep + g1
                    for g2 in next(os.walk(parent2))[1]:
                        leaf = parent2 + os.sep + g2
                        
                        # Apply filter
                        if ( fnmatch.fnmatch( leaf, filter ) ):
                            pkgs.append( leaf )
                    
            # Display package
            for i in pkgs:
                print(i.replace(common_args['-p']+os.sep,'',1))
            

    # List packages in the Native Package universe
    elif ( args['--native'] ):
        files = utils.get_file_list( filter, common_args['--uverse'] )
       
        tmark = _parse_datetime( common_args,  args )
    
        for f in files:
            if ( f.endswith('.top') ):
                info    = deps.read_spec_from_tar_file(f)[0]
                pname,e = os.path.splitext(os.path.basename(f))
            
                if ( args['--newer'] ):
                    if ( tmark > info['pubtime'] ):
                        continue
                elif ( args['--older'] ):
                    if ( tmark < info['pubtime'] ):
                        continue
                    
                if ( not args['--long'] ):
                    print(pname)
                else:
                    print("{}  {}".format( utils.epoch_secs_to_short_local(info['pubtime']), pname ))


        
    # List loaded packages
    else:
        xpkg = common_args['-w'] + os.sep + OUTCAST_XPKGS_DIRNAME()
        for d in os.listdir(xpkg):
            lpath   = xpkg + os.sep + d
            linksrc = symlinks.get_link_source(lpath,False)
            if ( linksrc != '' ):

                # Apply filter
                f = utils.get_pkg_source( lpath, common_args['-p'], common_args['-w'], OUTCAST_LOCAL_PKG_SUFFIX() )[1:]
                if ( not fnmatch.fnmatch( f, filter ) ):
                    pass
                    
                # Display package
                else:    
                    if ( args['--short'] ):
                        print(d)
                    else:
                        print ( f )




#------------------------------------------------------------------------------
def _parse_datetime( common_args, args ):
    # Get the datetime arg
    if ( args['--newer'] ):
        mark = args['--newer']
    elif ( args['--older'] ):
        mark = args['--older'] 
    else:
        return None

    # Case: Explicit date/time    
    if ( mark[0:1].isdigit() ):
        mark = utils.short_local_to_epoch_secs( mark )

    # Case: Use publish time of package
    else:
        f     = os.path.join(common_args['--uverse'], mark + '.top' )
        info  = deps.read_spec_from_tar_file(f)[0]
        mark  = info['pubtime']
    
    return mark
