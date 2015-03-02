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

# 
import os

# get definition of the Options strcuture
from nqbplib.base import BuildValues

# Get the location of the compiler toolchain
KPIT_ROOT = os.environ.get( 'KPIT_GNU_RX_ROOT' )
if ( KPIT_ROOT == None ):
    exit( "ERROR: The {} environment variable is not set.".format('KPIT_GNU_RX_ROOT') )



#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item     
FINAL_OUTPUT_NAME = 'a.x'

# Set the name of the linker script
LINKER_SCRIPT = 'ldscript-rx62n'

# Set reset vector/funciton name
RESET_VECTOR_NAME = '_PowerON_Reset'

# Define paths to needed libraries
LIB_PATH1 = os.path.join( KPIT_ROOT, "lib\\gcc\\rx-elf\\4.5-GNURX_v11.03\\64-bit-double" )
LIB_PATH2 = os.path.join( KPIT_ROOT, "rx-elf\\lib\\64-bit-double" )

# Define include path for the above libraries
INC_PATH1 = os.path.join( KPIT_ROOT, "lib\\gcc\\rx-elf\\4.5-GNURX_v11.03\\include" )
INC_PATH2 = os.path.join( KPIT_ROOT, "rx-elf\\include" )

#
# For build config/variant: "Release"
#

# Set project specific 'base' (i.e always used) options
base_release = BuildValues()        # Do NOT comment out this line
base_release.cflags    = ' -Wall -mlittle-endian-data -m64bit-doubles -mint-register=0 -DCPPAPP '
base_release.asmflags += base_release.cflags + '-x assembler-with-cpp ' 
base_release.cflags   += ' -x c++ '
base_release.inc       = ' -I ' + INC_PATH1  
base_release.inc      += ' -I ' + INC_PATH2
base_release.asminc    = base_release.inc

base_release.linkflags  = ' -T ..\\' + LINKER_SCRIPT + ' -e ' + RESET_VECTOR_NAME
base_release.linklibs   = ' -L ' + LIB_PATH1
base_release.linklibs  += ' -L ' + LIB_PATH2
#base_release.firstobjs = ' ..\\vector_table.o'
                                    
                                    
# Set project specific 'optimzed' options
optimzed_release = BuildValues()    # Do NOT comment out this line
optimzed_release.cflags   = ' -Os '
optimzed_release.asmflags = optimzed_release.cflags

# Set project specific 'debug' options
debug_release = BuildValues()       # Do NOT comment out this line
debug_release.cflags   = ' -g2 '
debug_release.asmflags = '-Wa,--gdwarf-2 '




#-------------------------------------------------
# ONLY edit this section if you are ADDING options
# for build configurations/variants OTHER than the
# 'release' build
#-------------------------------------------------

release_opts = { 'user_base':base_release, 
                 'user_optimized':optimzed_release, 
                 'user_debug':debug_release
               }
               
               
# Add new dictionary of for new build configuraiton options
#xyz_opts = { 'user_base':base_xyz, 
#             'user_optimized':optimzed_xyz, 
#             'user_debug':debug_xyz
#           }
  
        
# Add new variant option dictionary to # dictionary of 
# build varaints
build_variants = { 'target':release_opts,
#                  'xyz':xyz_opts,
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================



# Capture project/build directory
prjdir = os.path.dirname(os.path.abspath(__file__))


# Select Module that contains the desired toolcahin
from nqbplib.toolchains.windows.kpit_gnurx_elf.console_exe import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, "target" )
    return tc 
