#!/usr/bin/env bash
set -e

# This script is used by the CI/Build machine to build the Linux projects
#
# The script ASSUMES that the working directory is the package root
#
# usage: build_linux.sh <bldnum>
# 

# setup the environment
source ./env.sh default

# Build the Catch2 static library 
pushd projects
$NQBP_BIN/other/bob.py -v4 linux  -c --bld-all
popd

# Build all test linux projects
pushd tests
$NQBP_BIN/other/bob.py -v4 linux  --bld-all --bldtime --bldnum $1

# Run unit tests 
$NQBP_BIN/other/chuck.py -v --match a.out --dir _posix64
$NQBP_BIN/other/chuck.py -v --match aa.out --dir _posix64
$NQBP_BIN/other/chuck.py -v --match a.py --dir _posix64
$NQBP_BIN/other/chuck.py -v --match aa.py --dir _posix64

$NQBP_BIN/other/chuck.py -v --match a.out --dir _posix32
$NQBP_BIN/other/chuck.py -v --match aa.out --dir _posix32
$NQBP_BIN/other/chuck.py -v --match a.py --dir _posix32
$NQBP_BIN/other/chuck.py -v --match aa.py --dir _posix32


