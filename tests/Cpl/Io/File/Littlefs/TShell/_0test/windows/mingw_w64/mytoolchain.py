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
FINAL_OUTPUT_NAME = 'b.exe'


#
# For build config/variant: "Release"
#

# Set project specific 'base' (i.e always used) options
base_release              = BuildValues()        # Do NOT comment out this line
base_release.c_only_flags = '-std=c11'
base_release.cflags       = '-m32 -Wall -Werror -DLFS_THREADSAFE -DLFS_NO_MALLOC -DLFS_NAME_MAX=16' # -DLFS_FILEBD_YES_TRACE -DLFS_YES_TRACE'
base_release.cppflags     = '-std=c++11 '
base_release.linkflags    = '-m32 -fprofile-arcs'
base_release.linklibs     = '-lgcov'

# Set project specific 'optimized' options
optimzed_release           = BuildValues()    # Do NOT comment out this line
optimzed_release.cflags    = '-O3'
optimzed_release.linklibs  = '-lstdc++'

# Set project specific 'debug' options
debug_release           = BuildValues()       # Do NOT comment out this line
debug_release.linklibs  = '-lstdc++'




#
# For build config/variant: "win64"
# (note: uses same internal toolchain options as the 'Release' variant,
#        only the 'User' options will/are different)
#

# Construct option structs
base_win64     = BuildValues()
optimzed_win64 = BuildValues()
debug_win64    = BuildValues()

# Set 'base' options
base_win64.c_only_flags = '-std=c11'
base_win64.cflags       = '-m64 -Wall -Werror -DLFS_THREADSAFE -DLFS_NO_MALLOC -DLFS_NAME_MAX=16'
base_win64.cppflags     = '-std=c++11 '
base_win64.linkflags    = '-m64 -fprofile-arcs'
base_win64.linklibs     = '-lgcov'

# Set 'Optimized' options
optimzed_win64.cflags    = '-O3'
optimzed_win64.linklibs  = '-lstdc++'

# Set 'debug' options
debug_win64.linklibs  = '-lstdc++'

#-------------------------------------------------
# ONLY edit this section if you are ADDING options
# for build configurations/variants OTHER than the
# 'release' build
#-------------------------------------------------

release_opts = { 'user_base':base_release, 
                 'user_optimized':optimzed_release, 
                 'user_debug':debug_release
               }
               
# Add new dictionary of for new build configuration options
win64_opts = { 'user_base':base_win64,
               'user_optimized':optimzed_win64,
               'user_debug':debug_win64
             }
               
        
# Add new variant option dictionary to # dictionary of 
# build variants
build_variants = { 'win32':release_opts,
                   'win64':win64_opts
                 }    


#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================



# Capture project/build directory
import os
prjdir = os.path.dirname(os.path.abspath(__file__))


# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.mingw_w64.console_exe import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, "win32" )
    return tc 
