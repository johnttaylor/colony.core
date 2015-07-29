#!/usr/bin/python
"""Invokes NQBP's genfsm_base.py script.  To run 'GENFSM' copy this file to 
   your source directory.  Then edit the local script to generate one or more
   Finite State Machines (FSMs)
"""

import os
import sys

# Make sure the environment is properly set
NQBP_BIN = os.environ.get('NQBP_BIN')
if ( NQBP_BIN == None ):
    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
sys.path.append( NQBP_BIN )

# Find the Package & Workspace root
from other import genfsm_base

# Generate FSM#1, where [1]:= Diagram name, [2]:= Namespace(s).
sys.argv.append('-d 4')
sys.argv.append('Fsm')
sys.argv.append('Rte::Db::Record')
genfsm_base.run( sys.argv )

