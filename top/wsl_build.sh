#!/bin/bash
set -e

# This script is used by the CI/Build machine to build the Linux test
# project on a Windows machine by running the WSL (Window Subsystem for Linux)
#
# The script ASSUMES that the working directory is the package root
#
# usage: wsl_build.sh <bldnum>
#
# Example: wsl_build.sh 32

# setup the environment
source ./env.sh

# Build all linux projects
cd tests
$NQBP_BIN/other/bob.py -v linux  --bld-all -vt --bldnum $1
