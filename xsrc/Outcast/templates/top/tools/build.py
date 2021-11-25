#!/usr/bin/env python3
"""
Package specific script that 'builds' the package content as part of the 
publish process.  

Usage: build <pkgroot>

Returns: If the build is succesful, then the script returns '0'; else
         a non-zero value is returned.
"""

import sys
import os
import subprocess

#------------------------------------------------------------------------------
#
# This section can be deleted if NOT using the Outcast Bob builder tool.
# This method assumes that the file top/publish_build.lst exists and it
# contains the list of project(s) to build.
#
def _use_outcast():
    # Begin 
    if ( len(sys.argv) < 2 ):
        exit( "ERROR: Missing <pkgroot> command argument" )


    # Build arguments for the bob-the-builder
    buildlist = os.path.join( sys.argv[1], "top",   "publish_build.lst" )
    path      = os.path.join( sys.argv[1], "tests" )
    cmd       = "bob.py build -v --path {} --file {}".format(path, buildlist)

    # Invoke the build
    print("EXECUTING: " + cmd)
    p = subprocess.Popen( cmd, shell=True )
    r = p.communicate()
    if ( p.returncode != 0 ):
        exit( "ERROR: Build(s) failed" )
        

#------------------------------------------------------------------------------
# Uncomment out the following line if using the Outcast Bob builder tool
#_use_outcast()

# Delete this line if using the Outcast default namespace algorihtm
print("Building...")
