#---------------------------------------------------------------------------
# This python module is used to customize a supported toolchain for your 
# project specific settings.
#
# Notes:
#    - ONLY edit/add statements in the sections marked by BEGIN/END EDITS
#      markers.
#    - Maintain indentation level and use spaces (it's a python thing) 
#    - rvalues must be enclosed in quotes (single ' ' or double " ")
#    - The structure/class 'BuildValues' contains (at a minimum the
#      following data members.  Any member not specifically set defaults
#      to null/empty string
#            .inc 
#            .asminc
#            .cflags
#            .cppflags
#            .asmflags
#            .linkflags
#            .linklibs
#           
#---------------------------------------------------------------------------

# get definition of the Options structure
from nqbplib.base import BuildValues
from nqbplib.my_globals import NQBP_WORK_ROOT

#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item
FINAL_OUTPUT_NAME = 'a.exe'


#
# For build config/variant: "Release" 
#

# Set project specific 'base' (i.e always used) options
base_release              = BuildValues()        # Do NOT comment out this line
base_release.cflags       = '/W3 /WX /D LFS_THREADSAFE /D LFS_NO_MALLOC /D DLFS_NAME_MAX=16'
base_release.c_only_flags = '/std:c11'
base_release.cppflags     = '/EHsc'  # /EHsc enables exceptions


# Set project specific 'optimized' options
optimzed_release          = BuildValues()    # Do NOT comment out this line
optimzed_release.cflags   = '/O2'
optimzed_release.linklibs = ''

# Set project specific 'debug' options
debug_release          = BuildValues()       # Do NOT comment out this line
debug_release.cflags   = '/D "_MY_APP_DEBUG_SWITCH_"'
debug_release.linklibs = ''

#-------------------------------------------------
# ONLY edit this section if you are ADDING options
# for build configurations/variants OTHER than the
# 'release' build
#-------------------------------------------------

release_opts = { 'user_base':base_release, 
                 'user_optimized':optimzed_release, 
                 'user_debug':debug_release
               }
               
               
        
# Add new variant option dictionary to # dictionary of 
# build variants
build_variants = { 'win32':release_opts
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================



# Capture project/build directory
import os
prjdir = os.path.dirname(os.path.abspath(__file__))


# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.vc12.console_exe import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, 'win32' )
    return tc 
