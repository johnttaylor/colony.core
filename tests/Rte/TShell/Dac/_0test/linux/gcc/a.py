#!/usr/bin/python
"""
Script to run unittests (leverages NQBP python code base)

"""

import os
import sys
import shutil
import subprocess

#------------------------------------------------------------------------------
def run_test( root, subdir ):
    utils.push_dir( subdir )
    src = os.path.join( root, 'colony.core', 'src', 'Rte', 'TShell', 'Dac', '_0test', 'testscript.txt' )
    shutil.copy( src, '.' )
    print "Running unit test dir: {} ...".format( subdir )
    p = subprocess.Popen( 'b.out < testscript.txt', shell=True )
    p.communicate()
    if ( p.returncode != 0 ):
        exit( "FAILED test" )
    utils.pop_dir()
        


#------------------------------------------------------------------------------
# Make sure the environment is properly set
NQBP_BIN = os.environ.get('NQBP_BIN')
if ( NQBP_BIN == None ):
    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
sys.path.append( NQBP_BIN )

# Find the Package & Workspace root
from nqbplib import utils
from nqbplib import my_globals
utils.set_pkg_and_wrkspace_roots(__file__)


#------------------------------------------------------------------------------
# MAIN ENTRY POINT....
#

if ( os.path.isdir( '_posix' ) ):
    run_test( my_globals.NQBP_WORK_ROOT(), '_posix' )
    print

if ( os.path.isdir( '_posix64' ) ):
    run_test( my_globals.NQBP_WORK_ROOT(), '_posix64' )
    print

if ( os.path.isdir( '_cpp11' ) ):
    run_test( my_globals.NQBP_WORK_ROOT(), '_cpp11' )
    print