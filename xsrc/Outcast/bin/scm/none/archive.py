"""
 
Script that creates an archive file form the current SCM snapshot
===============================================================================
usage: evie [common-opts] archive [options] <pkgname> <tarname>
       evie [common-opts] archive --list
       
Arguments:
    <pkgname>           Name of the package to archive.  The pkgname must be
                        a 'local package' in the current Workspace.
    <tarname>           Name of the archive file.
    --prefix PREFIX     Prepend 'PREFIX' to each filename in the archive
    -d PATH             Top level directory to begin the archive from.  PATH is
                        relative to the package's root directory.
    --format FMT        Archive format: tar, tgz, zip, etc. [Default: tgz]
    --list              Returns a list of Archive formats supported
      
Options:
    -h, --help          Display help for this command

    
Notes:
    o Returns zero if the archive file is successfully created; else non-zero is
      returned.    
    o The script is responsible for filtering out and/or not including files 
      that are NOT being stored/tracked in the SCM repository, e.g. for git, the
      archive should NO include files specified by the .gitignore files. 
    o If a '.' is used for <pkgname> then <pkgname> is derived from the
      the current working directory where the command was invoked from.  
    
    
"""
import os
import utils
from docopt.docopt import docopt


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'archive', 'Creates an archive file from the current SCM snapshot' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    print("ARCHIVE: {}/{}".format(args['<pkgname>'], args['-d'] ))

    # Return 'error' since this is just a stub
    exit(1)
                 